#include "queue"
#include "esphome/core/log.h"
#include "example_component.h"

namespace esphome {
    namespace sofarsolar_inverter {

        struct RegisterTask {
            uint8_t register_index; // Index of the register to read
            bool operator<(const RegisterTask &other) const {
                return SofarSolar_Register[this.register_index][4] > SofarSolar_Register[other.register_index][4]; // Achtung: größer als = niedrigere Priorität!
            }
        };

        static const char *TAG = "sofarsolar_inverter.component";
        static const uint16_t SofarSolar_Register[][] = [
            // Register Address, Quantity, Type, Offset, Timer, Priority
            // Type: 0 uint16_t, 1 int16_t, 2 uint32_t, 3 int32_t, 4 float16, 5 float32
            // Priority: 0 - Low, 1 - Medium, 2 - High, 3 - Critical
            [0x0684, 2, 2, 0, 1], // PV Generation Today
            [0x0686, 2, 2, 0, 0], // PV Generation Total
            [0x0688, 2, 2, 0, 1], // Load Consumption Today
            [0x068A, 2, 2, 0, 0], // Load Consumption Total
            [0x0694, 2, 2, 0, 1], // Battery Charge Today
            [0x0696, 2, 2, 0, 0], // Battery Charge Total
            [0x0698, 2, 2, 0, 1], // Battery Discharge Today
            [0x069A, 2, 2, 0, 0], // Battery Discharge Total
            [0x0485, 1, 1, 0, 3], // Total Active Power Inverter
            [0x0584, 1, 0, 0, 2], // PV Voltage 1
            [0x0585, 1, 0, 0, 2], // PV Current 1
            [0x0586, 1, 0, 0, 2], // PV Power 1
            [0x0588, 1, 0, 0, 2], // PV Voltage 2
            [0x0589, 1, 0, 0, 2], // PV Current 2
            [0x058A, 1, 0, 0, 2], // PV Power 2
            [0x05C4, 1, 0, 0, 3], // PV Power Total
            [0x0667, 1, 1, 0, 3], // Battery Power Total
            [0x0668, 1, 0, 0, 1], // Battery State of Charge Total
            [0x1187, 2, 3, 0, 3], // Desired Grid Power
            [0x1189, 2, 3, 0, 3], // Minimum Battery Power
            [0x118B, 2, 3, 0, 3], // Maximum Battery Power
            [0x1110, 1, 0, 0, 0], // Energy Storage Mode
            [0x1044, 1, 0, 0, 0], // Battery Conf ID
            [0x1045, 1, 0, 0, 0], // Battery Conf Address
            [0x1046, 1, 0, 0, 0], // Battery Conf Protocol
            [0x1050, 1, 0, 0, 0], // Battery Conf Voltage Nominal
            [0x1047, 1, 0, 0, 0], // Battery Conf Voltage Over
            [0x1048, 1, 0, 0, 0], // Battery Conf Voltage Charge
            [0x1049, 1, 0, 0, 0], // Battery Conf Voltage Lack
            [0x104A, 1, 0, 0, 0], // Battery Conf Voltage Discharge Stop
            [0x104B, 1, 0, 0, 0], // Battery Conf Current Charge Limit
            [0x104C, 1, 0, 0, 0], // Battery Conf Current Discharge Limit
            [0x104D, 1, 0, 0, 0], // Battery Conf Depth of Discharge
            [0x104E, 1, 0, 0, 0], // Battery Conf End of Discharge
            [0x104F, 1, 0, 0, 0], // Battery Conf Capacity
            [0x1051, 1, 0, 0, 0], // Battery Conf Cell Type
            [0x1052, 1, 0, 0, 0], // Battery Conf EPS Buffer
            [0x1053, 1, 0, 0, 0] // Battery Conf Control
        ]

        int time_last_loop = 0;
        bool current_reading = false;
        int time_begin_reading = 0;
        std::priority_queue<RegisterTask> register_tasks;

        void SofarSolar_Inverter::setup() {
            // Code here should perform all component initialization,
            //  whether hardware, memory, or otherwise
        }

        void SofarSolar_Inverter::loop() {
            int time_since_last_loop = millis() - time_last_loop;
            time_last_loop = millis();
            for (int i = 0; i < sizeof(SofarSolar_Register) / sizeof(SofarSolar_Register[0]); i++) {
                if (SofarSolar_Register[i][4] > 0 && time_since_last_loop >= SofarSolar_Register[i][4] * 1000) {
                    // Create a task for the register
                    RegisterTask task;
                    task.register_index = i;
                    // Add the task to a priority queue
                    register_tasks.push(task);
                }
            }

            if (!current_reading && !register_tasks.empty()) {
                // Get the highest priority task
                RegisterTask task = register_tasks.top();
                current_reading = true;
                time_begin_reading = millis();
                send_read_modbus_registers(SofarSolar_Register[task.register_index][0], SofarSolar_Register[task.register_index][1]);
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
                    if (check_crc(response) & read_data[0] == 0x01 && response[1] == 0x03 && response[2] == quantity) {
                    // Process the response based on the register type
                        switch (SofarSolar_Register[i][2]) {
                            case 0: // uint16_t
                                uint16_t value = (response[3] << 8) | response[4];
                                break;
                            case 1: // int16_t
                                int16_t int_value = (response[3] << 8) | response[4];
                                break;
                            case 2: // uint32_t
                                uint32_t uint_value = (response[3] << 24) | (response[4] << 16) | (response[5] << 8) | response[6];
                                break;
                            case 3: // int32_t
                                int32_t int32_value = (response[3] << 24) | (response[4] << 16) | (response[5] << 8) | response[6];
                                break;
                            case 4: // float16
                                // Handle float16 response
                                break;
                            case 5: // float32
                                // Handle float32 response
                                break;
                            default:
                                ESP_LOGE(TAG, "Unknown register type");
                        }
                        switch (i) {
                            case 0: // PV Generation Today
                                if (this->pv_generation_today_sensor_ != nullptr) {
                                    this->pv_generation_today_sensor_->publish_state(value);
                                }
                                break;
                            case 1: // PV Generation Total
                                if (this->pv_generation_total_sensor_ != nullptr) {
                                    this->pv_generation_total_sensor_->publish_state(value);
                                }
                                break;
                            case 2: // Load Consumption Today
                                if (this->load_consumption_today_sensor_ != nullptr) {
                                    this->load_consumption_today_sensor_->publish_state(value);
                                }
                                break;
                            case 3: // Load Consumption Total
                                if (this->load_consumption_total_sensor_ != nullptr) {
                                    this->load_consumption_total_sensor_->publish_state(value);
                                }
                                break;
                            case 4: // Battery Charge Today
                                if (this->battery_charge_today_sensor_ != nullptr) {
                                    this->battery_charge_today_sensor_->publish_state(value);
                                }
                                break;
                            case 5: // Battery Charge Total
                                if (this->battery_charge_total_sensor_ != nullptr) {
                                    this->battery_charge_total_sensor_->publish_state(value);
                                }
                                break;
                            case 6: // Battery Discharge Today
                                if (this->battery_discharge_today_sensor_ != nullptr) {
                                    this->battery_discharge_today_sensor_->publish_state(value);
                                }
                                break;
                            case 7: // Battery Discharge Total
                                if (this->battery_discharge_total_sensor_ != nullptr) {
                                    this->battery_discharge_total_sensor_->publish_state(value);
                                }
                                break;
                            case 8: // Total Active Power Inverter
                                if (this->total_active_power_inverter_sensor_ != nullptr) {
                                    this->total_active_power_inverter_sensor_->publish_state(int_value);
                                }
                                break;
                            case 9: // PV Voltage 1
                                if (this->pv_voltage_1_sensor_ != nullptr) {
                                    this->pv_voltage_1_sensor_->publish_state(int_value / 10.0f); // Convert to volts
                                }
                                break;
                            case 10: // PV Current 1
                                if (this->pv_current_1_sensor_ != nullptr) {
                                    this->pv_current_1_sensor_->publish_state(int_value / 100.0f); // Convert to amps
                                }
                                break;
                            case 11: // PV Power 1
                                if (this->pv_power_1_sensor_ != nullptr) {
                                    this->pv_power_1_sensor_->publish_state(int_value / 10.0f); // Convert to watts
                                }
                                break;
                            case 12: // PV Voltage 2
                                if (this->pv_voltage_2_sensor_ != nullptr) {
                                    this->pv_voltage_2_sensor_->publish_state(int_value / 10.0f); // Convert to volts
                                }
                                break;
                            case 13: // PV Current 2
                                if (this->pv_current_2_sensor_ != nullptr) {
                                    this->pv_current_2_sensor_->publish_state(int_value / 100.0f); // Convert to amps
                                }
                                break;
                            case 14: // PV Power 2
                                if (this->pv_power_2_sensor_ != nullptr) {
                                    this->pv_power_2_sensor_->publish_state(int_value / 10.0f); // Convert to watts
                                }
                                break;
                            case 15: // PV Power Total
                                if (this->pv_power_total_sensor_ != nullptr) {
                                    this->pv_power_total_sensor_->publish_state(int_value / 10.0f); // Convert to watts
                                }
                                break;
                            case 16: // Battery Power Total
                                if (this->battery_power_total_sensor_ != nullptr) {
                                    this->battery_power_total_sensor_->publish_state(int_value / 10.0f); // Convert to watts
                                }
                                break;
                            case 17: // Battery State of Charge Total
                                if (this->battery_state_of_charge_total_sensor_ != nullptr) {
                                    this->battery_state_of_charge_total_sensor_->publish_state(int_value / 10.0f); // Convert to percentage
                                }
                                break;
                            case 18: // Desired Grid Power
                                if (this->desired_grid_power_sensor_ != nullptr) {
                                    this->desired_grid_power_sensor_->publish_state(int_value / 10.0f); // Convert to watts
                                }
                                break;
                            case 19: // Minimum Battery Power
                                if (this->minimum_battery_power_sensor_ != nullptr) {
                                    this->minimum_battery_power_sensor_->publish_state(int_value / 10.0f); // Convert to watts
                                }
                                break;
                            case 20: // Maximum Battery Power
                                if (this->maximum_battery_power_sensor_ != nullptr) {
                                    this->maximum_battery_power_sensor_->publish_state(int_value / 10.0f); // Convert to watts
                                }
                                break;
                            case 21: // Energy Storage Mode
                                if (this->energy_storage_mode_sensor_ != nullptr) {
                                    this->energy_storage_mode_sensor_->publish_state(int_value);
                                }
                                break;
                            case 22: // Battery Conf ID
                                if (this->battery_conf_id_sensor_ != nullptr) {
                                    this->battery_conf_id_sensor_->publish_state(int_value);
                                }
                                break;
                            case 23: // Battery Conf Address
                                if (this->battery_conf_address_sensor_ != nullptr) {
                                    this->battery_conf_address_sensor_->publish_state(int_value);
                                }
                                break;
                            case 24: // Battery Conf Protocol
                                if (this->battery_conf_protocol_sensor_ != nullptr) {
                                    this->battery_conf_protocol_sensor_->publish_state(int_value);
                                }
                                break;
                            case 25: // Battery Conf Voltage Nominal
                                if (this->battery_conf_voltage_nominal_sensor_ != nullptr) {
                                    this->battery_conf_voltage_nominal_sensor_->publish_state(int_value / 10.0f); // Convert to volts
                                }
                                break;
                            case 26: // Battery Conf Voltage Over
                                if (this->battery_conf_voltage_over_sensor_ != nullptr) {
                                    this->battery_conf_voltage_over_sensor_->publish_state(int_value / 10.0f); // Convert to volts
                                }
                                break;
                            case 27: // Battery Conf Voltage Charge
                                if (this->battery_conf_voltage_charge_sensor_ != nullptr) {
                                    this->battery_conf_voltage_charge_sensor_->publish_state(int_value / 10.0f); // Convert to volts
                                }
                                break;
                            case 28: // Battery Conf Voltage Lack
                                if (this->battery_conf_voltage_lack_sensor_ != nullptr) {
                                    this->battery_conf_voltage_lack_sensor_->publish_state(int_value / 10.0f); // Convert to volts
                                }
                                break;
                            case 29: // Battery Conf Voltage Discharge Stop
                                if (this->battery_conf_voltage_discharge_stop_sensor_ != nullptr) {
                                    this->battery_conf_voltage_discharge_stop_sensor_->publish_state(int_value / 10.0f); // Convert to volts
                                }
                                break;
                            case 30: // Battery Conf Current Charge Limit
                                if (this->battery_conf_current_charge_limit_sensor_ != nullptr) {
                                    this->battery_conf_current_charge_limit_sensor_->publish_state(int_value / 100.0f); // Convert to amps
                                }
                                break;
                            case 31: // Battery Conf Current Discharge Limit
                                if (this->battery_conf_current_discharge_limit_sensor_ != nullptr) {
                                    this->battery_conf_current_discharge_limit_sensor_->publish_state(int_value / 100.0f); // Convert to amps
                                }
                                break;
                            case 32: // Battery Conf Depth of Discharge
                                if (this->battery_conf_depth_of_discharge_sensor_ != nullptr) {
                                    this->battery_conf_depth_of_discharge_sensor_->publish_state(int_value / 10.0f); // Convert to percentage
                                }
                                break;
                            case 33: // Battery Conf End of Discharge
                                if (this->battery_conf_end_of_discharge_sensor_ != nullptr) {
                                    this->battery_conf_end_of_discharge_sensor_->publish_state(int_value / 10.0f); // Convert to percentage
                                }
                                break;
                            case 34: // Battery Conf Capacity
                                if (this->battery_conf_capacity_sensor_ != nullptr) {
                                    this->battery_conf_capacity_sensor_->publish_state(int_value / 10.0f); // Convert to ampere-hours
                                }
                                break;
                            case 35: // Battery Conf Cell Type
                                if (this->battery_conf_cell_type_sensor_ != nullptr) {
                                    this->battery_conf_cell_type_sensor_->publish_state(int_value);
                                }
                                break;
                            case 36: // Battery Conf EPS Buffer
                                if (this->battery_conf_eps_buffer_sensor_ != nullptr) {
                                    this->battery_conf_eps_buffer_sensor_->publish_state(int_value);
                                }
                                break;
                            case 37: // Battery Conf Control
                                if (this->battery_conf_control_sensor_ != nullptr) {
                                    this->battery_conf_control_sensor_->publish_state(int_value);
                                }
                                break;
                            default:
                                ESP_LOGE(TAG, "Unknown register index %d", i);
                        }
                    } else {
                        ESP_LOGE(TAG, "CRC check failed for register %04X", start_address);
                    }
                }
            }
        }

        void SofarSolar_Inverter::dump_config(){
            ESP_LOGCONFIG(TAG, "SofarSolar_Inverter");
            ESP_LOGCONFIG(TAG, "  model = %s", this->model_.c_str());
            ESP_LOGCONFIG(TAG, "  modbus_address = %i", this->modbus_address_);
            ESP_LOGCONFIG(TAG, "  uart_id = %s", this->uart_id_.c_str());
            ESP_LOGCONFIG(TAG, "  zero_export = %s", TRUEFALSE(this->zero_export_));
            if (!this->power_id_.empty()) {
                ESP_LOGCONFIG(TAG, "  power_id = %s", this->power_id_.c_str());
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