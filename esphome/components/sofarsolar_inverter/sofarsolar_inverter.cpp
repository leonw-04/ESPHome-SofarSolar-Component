#include "queue"
#include "sofarsolar_inverter.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
    namespace sofarsolar_inverter {
        static const char *TAG = "sofarsolar_inverter.component";

        int time_last_loop = 0;
        bool current_reading = false; // Pointer to the current reading task
        int time_begin_reading = 0;
        std::priority_queue<RegisterTask> register_tasks;

        void SofarSolar_Inverter::setup() {
            // Code here should perform all component initialization,
            //  whether hardware, memory, or otherwise
        }

        void SofarSolar_Inverter::loop() {
            int time_since_last_loop = millis() - time_last_loop;
            time_last_loop = millis();
            for (int i = 0; i < sizeof(registers_G3) / sizeof(registers_G3[0]); i++) {
                if (registers_G3[i].timer > 0 && time_since_last_loop >= registers_G3[i].timer * 1000) {
                    // Create a task for the register
                    RegisterTask task;
                    task.register_index = i;
                    task.inverter = this;
                    // Add the task to a priority queue
                    register_tasks.push(task);
                }
            }

            if (!current_reading && !register_tasks.empty()) {
                // Get the highest priority task
                RegisterTask task = register_tasks.top();
                current_reading = true;
                time_begin_reading = millis();
                send_read_modbus_registers(registers_G3[task.register_index].start_address, registers_G3[task.register_index].quantity);
            } else if (current_reading) {
                std::vector<uint8_t> response;
                receive_modbus_response(response);
                if (millis() - time_begin_reading > 250) {
                    ESP_LOGE(TAG, "Timeout while waiting for response");
                    current_reading = false;
                    return;
                }
                if (response.empty()) {
                    ESP_LOGE(TAG, "No response received");
                } else {
                    current_reading = false;
                    if (check_crc(response) & response[0] == 0x01 && response[1] == 0x03 && response[2] == registers_G3[register_tasks.top().register_index].quantity) {
                    // Process the response based on the register type
                        uint16_t start_address = registers_G3[register_tasks.top().register_index].start_address;
                        uint8_t quantity = registers_G3[register_tasks.top().register_index].quantity;
                        ESP_LOGD(TAG, "Received response for register %04X with quantity %d", start_address, quantity);
                        update_sensor(register_tasks.top().register_index, response, 3); // Offset 3 for Modbus response
                        register_tasks.pop(); // Remove the task from the queue
                    } else {
                        ESP_LOGE(TAG, "CRC check failed");
                    }
                }
            }
        }

        void SofarSolar_Inverter::update_sensor(uint8_t register_index, std::vector<uint8_t> &response, uint8_t offset) {
            // Update the sensor based on the register index and response data
            if (register_index < sizeof(registers_G3) / sizeof(registers_G3[0])) {
                switch (registers_G3[register_index].type) {
                    case 0: // uint16_t
                        if (this->registers_G3[register_index].sensor != nullptr) {
                            this->registers_G3[register_index].sensor->publish_state(uint16_t_from_bytes(response, offset));
                        }
                        break;
                    case 1: // int16_t
                        if (this->registers_G3[register_index].sensor != nullptr) {
                            this->registers_G3[register_index].sensor->publish_state(int16_t_from_bytes(response, offset));
                        }
                        break;
                    case 2: // uint32_t
                        if (this->registers_G3[register_index].sensor != nullptr) {
                            this->registers_G3[register_index].sensor->publish_state(uint32_t_from_bytes(response, offset));
                        }
                        break;
                    case 3: // int32_t
                        if (this->registers_G3[register_index].sensor != nullptr) {
                            this->registers_G3[register_index].sensor->publish_state(int32_t_from_bytes(response, offset));
                        }
                        break;
                    case 4: // float16
                        // Handle float16 response
                        break;
                    case 5: // float32
                        // Handle float32 response
                        break;
                    default:
                        ESP_LOGE(TAG, "Unknown register type for index %d", register_index);
                }
            } else {
                ESP_LOGE(TAG, "Register index out of bounds: %d", register_index);
            }
        }

        void SofarSolar_Inverter::dump_config(){
            ESP_LOGCONFIG(TAG, "SofarSolar_Inverter");
            ESP_LOGCONFIG(TAG, "  model = %s", this->model_.c_str());
            ESP_LOGCONFIG(TAG, "  modbus_address = %i", this->modbus_address_);
            ESP_LOGCONFIG(TAG, "  zero_export = %s", TRUEFALSE(this->zero_export_));
            if (!this->power_id_.empty()) {
                ESP_LOGCONFIG(TAG, "  power_id = %s", this->power_id_.c_str());
            }
        }

        void SofarSolar_Inverter::send_read_modbus_registers(uint16_t start_address, uint16_t quantity) {
            // Create Modbus frame for reading registers
            std::vector<uint8_t> frame = {static_cast<uint8_t>(this->modbus_address_), 0x03, static_cast<uint8_t>(start_address >> 8), static_cast<uint8_t>(start_address & 0xFF), static_cast<uint8_t>(quantity >> 8), static_cast<uint8_t>(quantity & 0xFF)};
            send_modbus(frame);
        }

        void SofarSolar_Inverter::receive_modbus_response(std::vector<uint8_t> &response) {
            // Read Modbus response from UART
            uart::UARTComponent *uart = this->get_uart();
            response.clear();
            response.push_back(uart->read_byte());
        }

        void SofarSolar_Inverter::send_modbus(std::vector<uint8_t> frame) {
            // Send Modbus frame over UART
            calculate_crc(frame);
            uart::UARTComponent *uart = this->get_uart();
            uart->write_array(frame);
            ESP_LOGD(TAG, "Sending Modbus frame: %s", frame_to_string(frame).c_str());
        }

        void SofarSolar_Inverter::calculate_crc(std::vector<uint8_t> frame) {
            uint16_t crc = 0xFFFF;  // Initialwert für CRC16-CCITT-FALSE

            // Iteriere über alle Bytes im Vektor
            for (size_t i = 0; i < frame.size(); i++) {
                crc ^= frame[i];  // XOR mit dem Byte

                // Führe die 8 Bit-Schiebeoperationen durch
                for (uint8_t bit = 0; bit < 8; bit++) {
                    if (crc & 0x0001) {
                        crc = (crc >> 1) ^ 0xA001;  // Polynom für Modbus CRC-16 (0xA001, reversed 0x8005)
                    } else {
                        crc >>= 1;
                    }
                }
            }
            frame.push_back((crc) & 0xFF);
            frame.push_back((crc) >> 8);
        }

        void SofarSolar_Inverter::check_crc(std::vector<uint8_t> frame) {
            if (frame.size() < 2) {
                ESP_LOGE(TAG, "Frame too short to check CRC");
                return false;
            }
            uint16_t crc_received = (frame[frame.size() - 1] << 8) | frame[frame.size() - 2];
            frame.pop_back();
            frame.pop_back();
            calculate_crc(frame);
            uint16_t crc_calculated = (frame[frame.size() - 1] << 8) | frame[frame.size() - 2];
            if (crc_received != crc_calculated) {
                ESP_LOGE(TAG, "CRC check failed: received %04X, calculated %04X", crc_received, crc_calculated);
                return false;
            } else {
                ESP_LOGD(TAG, "CRC check passed: %04X", crc_received);
                return true;
            }
        }
    }
}