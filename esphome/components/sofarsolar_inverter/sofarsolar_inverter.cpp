#include "queue"
#include "sofarsolar_inverter.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
    namespace sofarsolar_inverter {
        static const char *TAG = "sofarsolar_inverter.component";

        int time_last_loop = 0;
        bool current_reading = false; // Pointer to the current reading task
        bool current_zero_export_write = false; // Pointer to the current reading task
        uint64_t time_begin_modbus_operation = 0;
        uint64_t zero_export_last_update = 0;
        std::priority_queue<RegisterTask> register_tasks;

        void SofarSolar_Inverter::setup() {
            // Code here should perform all component initialization,
            //  whether hardware, memory, or otherwise
        }

        void SofarSolar_Inverter::loop() {
            if (!current_reading && !current_zero_export_write && this->zero_export_ && millis() - zero_export_last_update > 1000) { // Update zero export every second
                zero_export_last_update = millis();
                ESP_LOGD(TAG, "Updating zero export status");
                // Read the current zero export status
                int32_t new_desired_grid_power = this->total_active_power_inverter_sensor_->state + this->power_sensor_->state;
                int32_t minimum_battery_power = -5000;
                int32_t maximum_battery_power = 5000;
                std::vector<uint8_t> data = {
                    static_cast<uint8_t>(new_desired_grid_power >> 24), static_cast<uint8_t>(new_desired_grid_power >> 16),
                    static_cast<uint8_t>(new_desired_grid_power >> 8), static_cast<uint8_t>(new_desired_grid_power & 0xFF),
                    static_cast<uint8_t>(minimum_battery_power >> 24), static_cast<uint8_t>(minimum_battery_power >> 16),
                    static_cast<uint8_t>(minimum_battery_power >> 8), static_cast<uint8_t>(minimum_battery_power & 0xFF),
                    static_cast<uint8_t>(maximum_battery_power >> 24), static_cast<uint8_t>(maximum_battery_power >> 16),
                    static_cast<uint8_t>(maximum_battery_power >> 8), static_cast<uint8_t>(maximum_battery_power & 0xFF)
                };
                ESP_LOGD(TAG, "Current total active power inverter: %f W, Current power sensor: %f W, New desired grid power: %d W", this->total_active_power_inverter_sensor_->state, this->power_sensor_->state, new_desired_grid_power);
                current_zero_export_write = true; // Set the flag to indicate that a zero export write is in progress
                time_begin_modbus_operation = millis();
                this->empty_uart_buffer(); // Clear the UART buffer before sending a new request
                this->send_write_modbus_registers(registers_G3[18].start_address, 6, data); // Write the new desired grid power, minimum battery power, and maximum battery power
            }
            ESP_LOGVV(TAG, "Elements in register_tasks: %d", register_tasks.size());
            for (int i = 0; i < sizeof(registers_G3) / sizeof(registers_G3[0]); i++) {
                if (registers_G3[i].sensor == nullptr) {
                    ESP_LOGVV(TAG, "Sensor for register %d is not set", registers_G3[i].start_address);
                    continue;
                }
                ESP_LOGVV(TAG, "Checking register %04X: Time since last update: %d seconds, Update interval: %d seconds", registers_G3[i].start_address, millis() / 1000 - registers_G3[i].timer, registers_G3[i].update_interval);
                if (millis() - registers_G3[i].timer > registers_G3[i].update_interval * 1000 && !registers_G3[i].is_queued) {
                    registers_G3[i].timer = millis();
                    // Create a task for the register
                    RegisterTask task;
                    task.register_index = i;
                    task.inverter = this;
                    // Add the task to a priority queue
                    register_tasks.push(task);
                    registers_G3[i].is_queued = true;
                    ESP_LOGV(TAG, "Register %04X is queued for reading", registers_G3[i].start_address);
                }
            }

            if (!current_reading && !current_zero_export_write && !register_tasks.empty()) {
                // Get the highest priority task
                RegisterTask task = register_tasks.top();
                current_reading = true;
                time_begin_modbus_operation = millis();
                empty_uart_buffer(); // Clear the UART buffer before sending a new request
                send_read_modbus_registers(registers_G3[task.register_index].start_address, registers_G3[task.register_index].quantity);
            } else if (current_reading) {
                if (millis() - time_begin_modbus_operation > 500) { // Timeout after 500 ms
                    ESP_LOGE(TAG, "Timeout while waiting for response");
                    current_reading = false;
                    registers_G3[register_tasks.top().register_index].is_queued = false; // Mark the register as not queued anymore
                    register_tasks.pop(); // Remove the task from the queue
                    return;
                }
                std::vector<uint8_t> response;
                if (!receive_modbus_response(response, 3 + registers_G3[register_tasks.top().register_index].quantity * 2 + 2)) {
                    ESP_LOGE(TAG, "No response received");
                } else {
                    current_reading = false;
                    if (check_crc(response) && response[1] == 0x03) {
                    // Process the response based on the register type
                        uint16_t start_address = registers_G3[register_tasks.top().register_index].start_address;
                        uint8_t quantity = registers_G3[register_tasks.top().register_index].quantity;
                        ESP_LOGD(TAG, "Received response for register %04X with quantity %d", start_address, quantity);
                        update_sensor(register_tasks.top().register_index, response, 3); // Offset 3 for Modbus response
                        registers_G3[register_tasks.top().register_index].is_queued = false; // Mark the register as not queued anymore
                        register_tasks.pop(); // Remove the task from the queue
                    } else {
                        registers_G3[register_tasks.top().register_index].is_queued = false; // Mark the register as not queued anymore
                        register_tasks.pop(); // Remove the task from the queue
                        ESP_LOGE(TAG, "Invalid response");
                    }
                }
            } else if (current_zero_export_write) {
                if (millis() - time_begin_modbus_operation > 500) { // Timeout after 500 ms
                    ESP_LOGE(TAG, "Timeout while waiting for zero export write response");
                    current_zero_export_write = false;
                    return;
                }
                std::vector<uint8_t> response;
                if (!receive_modbus_response(response, 8)) {
                    ESP_LOGE(TAG, "No response received for zero export write");
                } else {
                    current_zero_export_write = false;
                    if (check_crc(response) && response[1] == 0x10 && response[2] ==registers_G3[18].start_address >> 8 && response[3] == (registers_G3[18].start_address & 0xFF) && response[4] == 0x00 && response[5] == 0x06) {
                        ESP_LOGD(TAG, "Zero export write successful");
                    } else {
                        ESP_LOGE(TAG, "Invalid response for zero export write");
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
                            if (registers_G3[register_index].scale < 1) {
                                this->registers_G3[register_index].sensor->publish_state(((float) uint16_t_from_bytes(response, offset) * registers_G3[register_index].scale));
                            } else {
                                this->registers_G3[register_index].sensor->publish_state(uint16_t_from_bytes(response, offset) * registers_G3[register_index].scale);
                            }

                        }
                        break;
                    case 1: // int16_t
                        if (this->registers_G3[register_index].sensor != nullptr) {
                            if (registers_G3[register_index].scale < 1) {
                                this->registers_G3[register_index].sensor->publish_state(((float) int16_t_from_bytes(response, offset) * registers_G3[register_index].scale));
                            } else {
                                this->registers_G3[register_index].sensor->publish_state(int16_t_from_bytes(response, offset) * registers_G3[register_index].scale);
                            }
                        }
                        break;
                    case 2: // uint32_t
                        if (this->registers_G3[register_index].sensor != nullptr) {
                            if (registers_G3[register_index].scale < 1) {
                                this->registers_G3[register_index].sensor->publish_state(((float) uint32_t_from_bytes(response, offset) * registers_G3[register_index].scale));
                            } else {
                                this->registers_G3[register_index].sensor->publish_state(uint32_t_from_bytes(response, offset) * registers_G3[register_index].scale);
                            }
                        }
                        break;
                    case 3: // int32_t
                        if (this->registers_G3[register_index].sensor != nullptr) {
                            if (registers_G3[register_index].scale < 1) {
                                this->registers_G3[register_index].sensor->publish_state(((float) int32_t_from_bytes(response, offset) * registers_G3[register_index].scale));
                            } else {
                                this->registers_G3[register_index].sensor->publish_state(int32_t_from_bytes(response, offset) * registers_G3[register_index].scale);
                            }
                        }
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
        }

        void SofarSolar_Inverter::send_read_modbus_registers(uint16_t start_address, uint16_t quantity) {
            // Create Modbus frame for reading registers
            std::vector<uint8_t> frame = {static_cast<uint8_t>(this->modbus_address_), 0x03, static_cast<uint8_t>(start_address >> 8), static_cast<uint8_t>(start_address & 0xFF), static_cast<uint8_t>(quantity >> 8), static_cast<uint8_t>(quantity & 0xFF)};
            send_modbus(frame);
        }

        void SofarSolar_Inverter::send_write_modbus_registers(uint16_t start_address, uint16_t quantity, const std::vector<uint8_t> &data) {
            // Create Modbus frame for writing registers
            std::vector<uint8_t> frame = {static_cast<uint8_t>(this->modbus_address_), 0x10, static_cast<uint8_t>(start_address >> 8), static_cast<uint8_t>(start_address & 0xFF), static_cast<uint8_t>(quantity >> 8), static_cast<uint8_t>(quantity & 0xFF), static_cast<uint8_t>(data.size())};
            frame.insert(frame.end(), data.begin(), data.end());
            send_modbus(frame);
        }

        bool SofarSolar_Inverter::receive_modbus_response(std::vector<uint8_t> &response, uint8_t response_length) {
            // Read Modbus response from UART
            if (this->peek() != -1 && this->available()) {
                ESP_LOGV(TAG, "Data available in UART buffer");
            } else {
                ESP_LOGV(TAG, "No data available in UART buffer");
                return false;
            }
            response.clear();
            uint8_t buffer[response_length];
            uint8_t i = 0;
            while (this->available()) {
                if (i >= response_length) {
                    ESP_LOGE(TAG, "Received more bytes than expected: %d", i);
                    empty_uart_buffer(); // Clear the buffer if too many bytes are received
                    return true;
                }
                if (!this->read_byte(&buffer[i])) {
                    ESP_LOGE(TAG, "Failed to read byte from UART");
                    return false;
                }
                i++;
            }
            response.insert(response.end(), buffer, buffer + response_length);
            ESP_LOGD(TAG, "Received Modbus response: %s", vector_to_string(response).c_str());
            return true;
        }

        void SofarSolar_Inverter::send_modbus(std::vector<uint8_t> frame) {
            // Send Modbus frame over UART
            calculate_crc(frame);
            ESP_LOGD(TAG, "Sending Modbus frame: %s", vector_to_string(frame).c_str());
            this->write_array(frame);
        }

        void SofarSolar_Inverter::calculate_crc(std::vector<uint8_t> &frame) {
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

        bool SofarSolar_Inverter::check_crc(std::vector<uint8_t> frame) {
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

        void SofarSolar_Inverter::empty_uart_buffer() {
            ESP_LOGVV(TAG, "Bytes vor leeren: %d", this->available());
            uint8_t byte;
            while (this->available()) {
                this->read_byte(&byte);
            }
            ESP_LOGVV(TAG, "Bytes nach leeren: %d", this->available());
        }
    }
}