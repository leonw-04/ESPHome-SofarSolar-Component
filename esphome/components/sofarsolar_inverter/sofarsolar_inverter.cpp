#include "queue"
#include "sofarsolar_inverter.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
    namespace sofarsolar_inverter {
        static const char *TAG = "sofarsolar_inverter.component";

        struct SofarSolar_Register {
            uint16_t start_address; // Start address of the register
            uint16_t quantity; // Number of registers to read
            uint8_t type; // Type of the register (0: uint16_t, 1: int16_t, 2: uint32_t, 3: int32_t, 4: float16, 5: float32)
            uint8_t priority; // Priority of the register for reading
            float scale; // Scale factor for the register value
            uint16_t update_interval = 0; // Update interval in seconds for the register
            uint64_t timer = 0; // Timer in seconds for reading the register
            SofarSolar_RegisterValue write_value; // Current value of the register
			SofarSolar_RegisterValue default_value; // Default value of the register
            bool write_set_value = false; // Flag to indicate if the register has a write value
		    bool is_default_value_set = false; // Flag to check if default value is set
			bool enforce_default_value = false; // Flag to enforce default value
            sensor::Sensor *sensor = nullptr; // Pointer to the sensor to update
            bool is_queued = false; // Flag to indicate if the register is queued for reading
			bool writeable;
			void (*write_funktion)(register_write_task) = nullptr; // Function pointer for writing to the register
            SofarSolar_Register(uint16_t start_address, uint16_t quantity, uint8_t type, uint8_t priority, float scale, bool writeable, void (*write_funktion)(register_write_task) = nullptr) : start_address(start_address), quantity(quantity), type(type), priority(priority), scale(scale), writeable(writeable), write_funktion(write_funktion) {}
        };

		struct register_read_task {
			SofarSolar_Register *register_ptr; // Pointer to the register to read
		    SofarSolar_RegisterValue read_value; // Value to read
            bool operator<(const register_read_task &other) const {
                return this->register_ptr->priority > other.register_ptr->priority;
            }
		};

        struct register_write_task {
			SofarSolar_Register *register_ptr; // Pointer to the register to read
            uint8_t number_of_registers; // Length of the data to write
        };

        int time_last_loop = 0;
        bool current_reading = false; // Pointer to the current reading task
        register_write_task current_write; // Pointer to the current write task
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
                registers_G3[DESIRED_GRID_POWER].write_value.int32_value = this->total_active_power_inverter_sensor_->state + this->power_sensor_->state;
                registers_G3[MINIMUM_BATTERY_POWER].write_value.int32_value = -5000;
                registers_G3[MINIMUM_BATTERY_POWER].write_value.int32_value = 5000;
                ESP_LOGD(TAG, "Current total active power inverter: %f W, Current power sensor: %f W, New desired grid power: %d W", this->total_active_power_inverter_sensor_->state, this->power_sensor_->state, new_desired_grid_power);
                register_write_task data;
                data.start_address = registers_G3[DESIRED_GRID_POWER].start_address;
                data.quantity = 6; // Number of registers to write
                current_write = data; // Set the flag to indicate that a zero export write is in progress
                time_begin_modbus_operation = millis();
                this->empty_uart_buffer(); // Clear the UART buffer before sending a new request
                this->write_desired_grid_power(); // Write the new desired grid power, minimum battery power, and maximum battery power
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
                if (check_for_response()) {
                    current_reading = false;
                    if (read_response(response, registers_G3[register_tasks.top().register_index])) {
                        SofarSolar_RegisterValue value;
                        value.uint64_value = extract_data_from_response(response);
                        if (registers_G3[register_tasks.top().register_index].is_default_value_set) {
                            if (value.uint64_value != registers_G3[register_tasks.top().register_index].default_value.uint64_value) { // Use default value if set
                                current_write = registers_G3[register_tasks.top().register_index]; // Set the flag to indicate that a write operation is in progress
                                time_begin_modbus_operation = millis();
                                registers_G3[register_tasks.top().register_index].write_funktion(registers_G3[register_tasks.top().register_index].start_address, registers_G3[register_tasks.top().register_index].quantity, registers_G3[register_tasks.top().register_index].default_value); // Call the write function if the value is not equal to the default value
                            }
                        }
                        update_sensor(register_tasks.top().register_index, value);
                    } else {
                        registers_G3[register_tasks.top().register_index].is_queued = false; // Mark the register as not queued anymore
                        register_tasks.pop(); // Remove the task from the queue
                        ESP_LOGE(TAG, "Invalid response");
                    }
                } else {
                    ESP_LOGE(TAG, "No response received");
                }
            } else if (current_write) {
                if (millis() - time_begin_modbus_operation > 500) { // Timeout after 500 ms
                    ESP_LOGE(TAG, "Timeout while waiting for zero export write response");
                    current_write = nullptr;
                    return;
                }
                std::vector<uint8_t> response;
                if (!check_for_response()) {
                    ESP_LOGE(TAG, "No response received for zero export write");
                } else {
                    current_write = nullptr;
                    if (write_response(response)) {
                        ESP_LOGD(TAG, "Write successful");
                    } else {
                        ESP_LOGE(TAG, "Invalid response for write");
                    }
                }
            }
        }

        bool check_for_response() {
            // Check if there is a response available in the UART buffer
            if (this->available() < 5) {
                return false; // Not enough bytes for a valid response
            }
            return this->peek() != -1;
        }

        bool read_response(std::vector<uint8_t> &response, SofarSolar_Register &register_info) {
            // Read the response from the UART buffer
            response.clear();
            while (this->available() > 0 && response.size()) {
                uint8_t byte = this->read();
                response.push_back(byte);
            }
            if (!check_crc(response)) { // Check CRC after reading the response
                return false; // Invalid CRC
            }
            if (!check_for_error_code(response)) {
                return false; // Error code present in the response
            }
            if response.data()[1] != 0x03) {
                ESP_LOGE(TAG, "Invalid Modbus response function code: %02X", response.data()[1]);
                response.clear(); // Clear the response on invalid function code
                return false; // Invalid function code
            }
            if (response.data()[2] != response.size() - 5 && response.data()[2] != register_info.quantity * 2) {
                ESP_LOGE(TAG, "Invalid response size: expected %d, got %d", response.data()[2], response.size() - 5);
                response.clear(); // Clear the response on invalid size
                return false; // Invalid response size
            }
            return true; // Valid read response
        }

        uint64_t extract_data_from_response(std::vector<uint8_t> &response) {
            switch (response.data()[2]) {
                case 2:
                    return (response.data()[3] << 8) | response.data()[4]; // 2 bytes for start address
                case 4:
                    return (response.data()[3] << 24) | (response.data()[4] << 16) | (response.data()[5] << 8) | response.data()[6]; // 4 bytes for start address
                case 8:
                    return (response.data()[3] << 56) | (response.data()[4] << 48) | (response.data()[5] << 40) | (response.data()[6] << 32) |
                           (response.data()[7] << 24) | (response.data()[8] << 16) | (response.data()[9] << 8) | response.data()[10]; // 8 bytes for start address
                default:
                    ESP_LOGE(TAG, "Invalid response size for extracting value");
                    return false; // Invalid response
        }

        bool write_response(SofarSolar_Register &register_info) {
            // Read the response from the UART buffer
            response.clear();
            while (this->available() > 0 && response.size()) {
                uint8_t byte = this->read();
                response.push_back(byte);
            }
            if (!check_crc(response)) { // Check CRC after reading the response
                return false; // Invalid CRC
            }
            if (!check_for_error_code(response)) {
                return false; // Error code present in the response
            }
            if response.data()[1] != 0x10) {
                ESP_LOGE(TAG, "Invalid Modbus response function code: %02X", response.data()[1]);
                response.clear(); // Clear the response on invalid function code
                return false; // Invalid function code
            }
            if (registers_G3[register_info.register_index].start_address != response.data()[2] << 8 | response.data()[3] && registers_G3[register_info.register_index].quantity != response.data()[4]) {
                ESP_LOGE(TAG, "Invalid response size: expected %04X, got %02X%02X", registers_G3[register_info.register_index].start_address, response.data()[2], response.data()[3]);
                return false; // Invalid response size
            }
            return true; // Valid write response
        }

        bool check_for_error_code(const std::vector<uint8_t> &response) {
            // Check if the response contains an error code
            if (response.data()[1] == 0x90) {
                switch (response.data()[2]) {
                    case 0x01:
                        ESP_LOGE(TAG, "Modbus error: Illegal function");
                        break;
                    case 0x02:
                        ESP_LOGE(TAG, "Modbus error: Illegal data address");
                        break;
                    case 0x03:
                        ESP_LOGE(TAG, "Modbus error: Illegal data value");
                        break;
                    case 0x04:
                        ESP_LOGE(TAG, "Modbus error: Slave equipment failure");
                        break;
                    case 0x07:
                        ESP_LOGE(TAG, "Modbus error: Busy with slave equipment");
                        break;
                    default:
                        ESP_LOGE(TAG, "Modbus error: Unknown error code %02X", response.data()[2]);
                }
                response.clear(); // Clear the response on error
                return false; // Modbus error response
            }
            return true; // No error code present
        }

        void SofarSolar_Inverter::write_desired_grid_power(register_write_task &task) {
            // Write the desired grid power, minimum battery power, and maximum battery power
            int32_t new_desired_grid_power;
            if (registers_G3[DESIRED_GRID_POWER].enforce_default_value && registers_G3[DESIRED_GRID_POWER].is_default_value_set) {
                new_desired_grid_power = registers_G3[DESIRED_GRID_POWER].default_value.int32_value;
            } else if (registers_G3[DESIRED_GRID_POWER].write_set_value) {
                new_desired_grid_power = registers_G3[DESIRED_GRID_POWER].write_value.int32_value;
            } else {
                new_desired_grid_power = registers_G3[DESIRED_GRID_POWER].sensor->state;
            }
            int32_t new_minimum_battery_power;
            if (registers_G3[MINIMUM_BATTERY_POWER].enforce_default_value && registers_G3[MINIMUM_BATTERY_POWER].is_default_value_set) {
                new_minimum_battery_power = registers_G3[MINIMUM_BATTERY_POWER].default_value.int32_value;
            } else if (registers_G3[DESIRED_GRID_POWER].write_set_value) {
                new_minimum_battery_power = registers_G3[MINIMUM_BATTERY_POWER].write_value.int32_value;
            } else {
                new_minimum_battery_power = registers_G3[MINIMUM_BATTERY_POWER].sensor->state;
            }
            int32_t new_maximum_battery_power;
            if (registers_G3[MAXIMUM_BATTERY_POWER].enforce_default_value && registers_G3[DESIRED_GRID_POWER].is_default_value_set) {
                new_maximum_battery_power = registers_G3[MAXIMUM_BATTERY_POWER].default_value.int32_value;
            } else if (registers_G3[DESIRED_GRID_POWER].write_set_value) {
                new_maximum_battery_power = registers_G3[MAXIMUM_BATTERY_POWER].write_value.int32_value;
            } else {
                new_maximum_battery_power = registers_G3[MAXIMUM_BATTERY_POWER].sensor->state;
            }
            ESP_LOGD(TAG, "Writing desired grid power: %d W", registers_G3[DESIRED_GRID_POWER].write_value.int32_value);
            ESP_LOGD(TAG, "Writing minimum battery power: %d W", registers_G3[MINIMUM_BATTERY_POWER].write_value.int32_value);
            ESP_LOGD(TAG, "Writing maximum battery power: %d W", registers_G3[MAXIMUM_BATTERY_POWER].write_value.int32_value);
            std::vector<uint8_t> data;
            data.push_back(static_cast<uint8_t>(new_desired_grid_power >> 24));
            data.push_back(static_cast<uint8_t>(new_desired_grid_power >> 16));
            data.push_back(static_cast<uint8_t>(new_desired_grid_power >> 8));
            data.push_back(static_cast<uint8_t>(new_desired_grid_power & 0xFF));
            data.push_back(static_cast<uint8_t>(new_minimum_battery_power >> 24));
            data.push_back(static_cast<uint8_t>(new_minimum_battery_power >> 16));
            data.push_back(static_cast<uint8_t>(new_minimum_battery_power >> 8));
            data.push_back(static_cast<uint8_t>(new_minimum_battery_power & 0xFF));
            data.push_back(static_cast<uint8_t>(new_maximum_battery_power >> 24));
            data.push_back(static_cast<uint8_t>(new_maximum_battery_power >> 16));
            data.push_back(static_cast<uint8_t>(new_maximum_battery_power >> 8));
            data.push_back(static_cast<uint8_t>(new_maximum_battery_power & 0xFF));
            uint16_t address = registers_G3[DESIRED_GRID_POWER].start_address;
            uint16_t quantity = registers_G3[DESIRED_GRID_POWER].quantity + registers_G3[MINIMUM_BATTERY_POWER].quantity + registers_G3[MAXIMUM_BATTERY_POWER].quantity;
            send_write_modbus_registers(address, quantity, data);
        }

        void SofarSolar_Inverter::write_battery_conf(register_write_task &task) {
            // Write the battery configuration
            ESP_LOGD(TAG, "Writing battery configuration");
            uint16_t new_battery_conf_id;
            if (registers_G3[BATTERY_CONF_ID].enforce_default_value && registers_G3[BATTERY_CONF_ID].is_default_value_set) {
                new_battery_conf_id = registers_G3[BATTERY_CONF_ID].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_ID].write_set_value) {
                new_battery_conf_id = registers_G3[BATTERY_CONF_ID].write_value.uint16_value;
            } else {
                new_battery_conf_id = registers_G3[BATTERY_CONF_ID].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_id >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_id & 0xFF));
            uint16_t new_battery_conf_address;
            if (registers_G3[BATTERY_CONF_ADDRESS].enforce_default_value && registers_G3[BATTERY_CONF_ADDRESS].is_default_value_set) {
                new_battery_conf_address = registers_G3[BATTERY_CONF_ADDRESS].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_ADDRESS].write_set_value) {
                new_battery_conf_address = registers_G3[BATTERY_CONF_ADDRESS].write_value.uint16_value;
            } else {
                new_battery_conf_address = registers_G3[BATTERY_CONF_ADDRESS].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_address >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_address & 0xFF));
            uint16_t new_battery_conf_protocol;
            if (registers_G3[BATTERY_CONF_PROTOCOL].enforce_default_value && registers_G3[BATTERY_CONF_PROTOCOL].is_default_value_set) {
                new_battery_conf_protocol = registers_G3[BATTERY_CONF_PROTOCOL].default_value.uint16_t_value;
            } else if (registers_G3[BATTERY_CONF_PROTOCOL].write_set_value) {
                new_battery_conf_protocol = registers_G3[BATTERY_CONF_PROTOCOL].write_value.uint16_t_value;
            } else {
                new_battery_conf_protocol = registers_G3[BATTERY_CONF_PROTOCOL].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_protocol >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_protocol & 0xFF));
            uint16_t new_battery_conf_voltage_over;
            if (registers_G3[BATTERY_CONF_VOLTAGE_OVER].enforce_default_value && registers_G3[BATTERY_CONF_VOLTAGE_OVER].is_default_value_set) {
                new_battery_conf_voltage_over = registers_G3[BATTERY_CONF_VOLTAGE_OVER].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_VOLTAGE_OVER].write_set_value) {
                new_battery_conf_voltage_over = registers_G3[BATTERY_CONF_VOLTAGE_OVER].write_value.uint16_value;
            } else {
                new_battery_conf_voltage_over = registers_G3[BATTERY_CONF_VOLTAGE_OVER].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_over >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_over & 0xFF));
            uint16_t new_battery_conf_voltage_charge;
            if (registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].enforce_default_value && registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].is_default_value_set) {
                new_battery_conf_voltage_charge = registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].write_set_value) {
                new_battery_conf_voltage_charge = registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].write_value.uint16_value;
            } else {
                new_battery_conf_voltage_charge = registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_charge >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_charge & 0xFF));
            uint16_t new_battery_conf_voltage_lack;
            if (registers_G3[BATTERY_CONF_VOLTAGE_LACK].enforce_default_value && registers_G3[BATTERY_CONF_VOLTAGE_LACK].is_default_value_set) {
                new_battery_conf_voltage_lack = registers_G3[BATTERY_CONF_VOLTAGE_LACK].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_VOLTAGE_LACK].write_set_value) {
                new_battery_conf_voltage_lack = registers_G3[BATTERY_CONF_VOLTAGE_LACK].write_value.uint16_value;
            } else {
                new_battery_conf_voltage_lack = registers_G3[BATTERY_CONF_VOLTAGE_LACK].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_lack >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_lack & 0xFF));
            uint16_t new_battery_conf_voltage_discharge_stop;
            if (registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].enforce_default_value && registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].is_default_value_set) {
                new_battery_conf_voltage_discharge_stop = registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].write_set_value) {
                new_battery_conf_voltage_discharge_stop = registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].write_value.uint16_value;
            } else {
                new_battery_conf_voltage_discharge_stop = registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_discharge_stop >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_discharge_stop & 0xFF));
            uint16_t new_battery_conf_current_charge_limit;
            if (registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].enforce_default_value && registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].is_default_value_set) {
                new_battery_conf_current_charge_limit = registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].write_set_value) {
                new_battery_conf_current_charge_limit = registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].write_value.uint16_value;
            } else {
                new_battery_conf_current_charge_limit = registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_charge_limit >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_charge_limit & 0xFF));
            uint16_t new_battery_conf_current_discharge_limit;
            if (registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].enforce_default_value && registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].is_default_value_set) {
                new_battery_conf_current_discharge_limit = registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].write_set_value) {
                new_battery_conf_current_discharge_limit = registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].write_value.uint16_value;
            } else {
                new_battery_conf_current_discharge_limit = registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_discharge_limit >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_discharge_limit & 0xFF));
            uint16_t new_battery_conf_depth_of_discharge;
            if (registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].enforce_default_value && registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].is_default_value_set) {
                new_battery_conf_depth_of_discharge = registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].write_set_value) {
                new_battery_conf_depth_of_discharge = registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].write_value.uint16_value;
            } else {
                new_battery_conf_depth_of_discharge = registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_depth_of_discharge >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_depth_of_discharge & 0xFF));
            uint16_t new_battery_conf_end_of_discharge;
            if (registers_G3[BATTERY_CONF_END_OF_DISCHARGE].enforce_default_value && registers_G3[BATTERY_CONF_END_OF_DISCHARGE].is_default_value_set) {
                new_battery_conf_end_of_discharge = registers_G3[BATTERY_CONF_END_OF_DISCHARGE].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_END_OF_DISCHARGE].write_set_value) {
                new_battery_conf_end_of_discharge = registers_G3[BATTERY_CONF_END_OF_DISCHARGE].write_value.uint16_value;
            } else {
                new_battery_conf_end_of_discharge = registers_G3[BATTERY_CONF_END_OF_DISCHARGE].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_end_of_discharge >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_end_of_discharge & 0xFF));
            uint16_t new_battery_conf_capacity;
            if (registers_G3[BATTERY_CONF_CAPACITY].enforce_default_value && registers_G3[BATTERY_CONF_CAPACITY].is_default_value_set) {
                new_battery_conf_capacity = registers_G3[BATTERY_CONF_CAPACITY].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_CAPACITY].write_set_value) {
                new_battery_conf_capacity = registers_G3[BATTERY_CONF_CAPACITY].write_value.uint16_value;
            } else {
                new_battery_conf_capacity = registers_G3[BATTERY_CONF_CAPACITY].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_capacity >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_capacity & 0xFF));
            uint16_t new_battery_conf_voltage_nominal;
            if (registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].enforce_default_value && registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].is_default_value_set) {
                new_battery_conf_voltage_nominal = registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].write_set_value) {
                new_battery_conf_voltage_nominal = registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].write_value.uint16_value;
            } else {
                new_battery_conf_voltage_nominal = registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_nominal >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_nominal & 0xFF));
            uint16_t new_battery_conf_cell_type;
            if (registers_G3[BATTERY_CONF_CELL_TYPE].enforce_default_value && registers_G3[BATTERY_CONF_CELL_TYPE].is_default_value_set) {
                new_battery_conf_cell_type = registers_G3[BATTERY_CONF_CELL_TYPE].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_CELL_TYPE].write_set_value) {
                new_battery_conf_cell_type = registers_G3[BATTERY_CONF_CELL_TYPE].write_value.uint16_value;
            } else {
                new_battery_conf_cell_type = registers_G3[BATTERY_CONF_CELL_TYPE].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_cell_type >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_cell_type & 0xFF));
            uint16_t new_battery_conf_eps_buffer;
            if (registers_G3[BATTERY_CONF_EPS_BUFFER].enforce_default_value && registers_G3[BATTERY_CONF_EPS_BUFFER].is_default_value_set) {
                new_battery_conf_eps_buffer = registers_G3[BATTERY_CONF_EPS_BUFFER].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_EPS_BUFFER].write_set_value) {
                new_battery_conf_eps_buffer = registers_G3[BATTERY_CONF_EPS_BUFFER].write_value.uint16_value;
            } else {
                new_battery_conf_eps_buffer = registers_G3[BATTERY_CONF_EPS_BUFFER].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_eps_buffer >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_eps_buffer & 0xFF));
            data.push_back(static_cast<uint8_t>(0x01 >> 8));
            data.push_back(static_cast<uint8_t>(0x01 & 0xFF)); // Write the battery configuration
        }

        void SofarSolar_Inverter::write_single_register(register_write_task &task) {
            SofarSolar_RegisterValue value;
            if (task.register_ptr.enforce_default_value && task.register_ptr.is_default_value_set) {
                value = task.register_ptr.default_value; // Use default value if set
            } else if (task.register_ptr.write_set_value) {
                value = task.register_ptr.write_value; // Use write value if set
            } else {
                value = task.register_ptr.sensor->state; // Use sensor value
            }
            std::vector<uint8_t> data;
            uint8_t data_length;
            switch (task.register_ptr.type) {
                case 0: // uint16_t
                    data.push_back(static_cast<uint8_t>(value.uint16_t >> 8));
                    data.push_back(static_cast<uint8_t>(value.uint16_t & 0xFF));
                    break;
                case 1: // int16_t
                    data.push_back(static_cast<uint8_t>(value.int16_t >> 8));
                    data.push_back(static_cast<uint8_t>(value.int16_t & 0xFF));
                    break;
                case 2: // uint32_t
                    data.push_back(static_cast<uint8_t>(value.uint32_t >> 24));
                    data.push_back(static_cast<uint8_t>(value.uint32_t >> 16));
                    data.push_back(static_cast<uint8_t>(value.uint32_t >> 8));
                    data.push_back(static_cast<uint8_t>(value.uint32_t & 0xFF));
                    break;
                case 3: // int32_t
                    data.push_back(static_cast<uint8_t>(value.int32_t >> 24));
                    data.push_back(static_cast<uint8_t>(value.int32_t >> 16));
                    data.push_back(static_cast<uint8_t>(value.int32_t >> 8));
                    data.push_back(static_cast<uint8_t>(value.int32_t & 0xFF));
                    break;
                case 4: // uint64_t
                    data.push_back(static_cast<uint8_t>(value.uint64_value >> 56));
                    data.push_back(static_cast<uint8_t>(value.uint64_value >> 48));
                    data.push_back(static_cast<uint8_t>(value.uint64_value >> 40));
                    data.push_back(static_cast<uint8_t>(value.uint64_value >> 32));
                    data.push_back(static_cast<uint8_t>(value.uint64_value >> 24));
                    data.push_back(static_cast<uint8_t>(value.uint64_value >> 16));
                    data.push_back(static_cast<uint8_t>(value.uint64_value >> 8));
                    data.push_back(static_cast<uint8_t>(value.uint64_value & 0xFF));
                    break;
                case 5: // int64_t
                    data.push_back(static_cast<uint8_t>(value.int64_value >> 56));
                    data.push_back(static_cast<uint8_t>(value.int64_value >> 48));
                    data.push_back(static_cast<uint8_t>(value.int64_value >> 40));
                    data.push_back(static_cast<uint8_t>(value.int64_value >> 32));
                    data.push_back(static_cast<uint8_t>(value.int64_value >> 24));
                    data.push_back(static_cast<uint8_t>(value.int64_value >> 16));
                    data.push_back(static_cast<uint8_t>(value.int64_value >> 8));
                    data.push_back(static_cast<uint8_t>(value.int64_value & 0xFF));
                    break;
                case 6: // float
                    uint32_t float_value;
                    memcpy(&float_value, &value.float_value, sizeof(float_value));
                    data.push_back(static_cast<uint8_t>(float_value >> 24));
                    data.push_back(static_cast<uint8_t>(float_value >> 16));
                    data.push_back(static_cast<uint8_t>(float_value >> 8));
                    data.push_back(static_cast<uint8_t>(float_value & 0xFF));
                    break;
                case 7: // double
                    uint64_t double_value;
                    memcpy(&double_value, &value.double_value, sizeof(double_value));
                    data.push_back(static_cast<uint8_t>(double_value >> 56));
                    data.push_back(static_cast<uint8_t>(double_value >> 48));
                    data.push_back(static_cast<uint8_t>(double_value >> 40));
                    data.push_back(static_cast<uint8_t>(double_value >> 32));
                    data.push_back(static_cast<uint8_t>(double_value >> 24));
                    data.push_back(static_cast<uint8_t>(double_value >> 16));
                    data.push_back(static_cast<uint8_t>(double_value >> 8));
                    data.push_back(static_cast<uint8_t>(double_value & 0xFF));
                    break;
                default:
                    ESP_LOGE(TAG, "Unknown register type for writing: %d", task.register_ptr.type);
                    return; // Exit if the register type is unknown
            }
            send_write_modbus_registers(task.register_ptr.start_address, task.quantity, data);
        }



        void SofarSolar_Inverter::update_sensor(uint8_t register_index, SofarSolar_RegisterValue &response) {
            // Update the sensor based on the register index and response data
            switch (registers_G3[register_index].type) {
                case 0: // uint16_t
                    if (this->registers_G3[register_index].sensor != nullptr) {
                        if (registers_G3[register_index].scale < 1) {
                            this->registers_G3[register_index].sensor->publish_state(((float) response.uint16_t * registers_G3[register_index].scale));
                        } else {
                            this->registers_G3[register_index].sensor->publish_state(response.uint16_t * registers_G3[register_index].scale);
                        }

                    }
                    break;
                case 1: // int16_t
                    if (this->registers_G3[register_index].sensor != nullptr) {
                        if (registers_G3[register_index].scale < 1) {
                            this->registers_G3[register_index].sensor->publish_state(((float) response.int16_t * registers_G3[register_index].scale));
                        } else {
                            this->registers_G3[register_index].sensor->publish_state(response.int16_t * registers_G3[register_index].scale);
                        }
                    }
                    break;
                case 2: // uint32_t
                    if (this->registers_G3[register_index].sensor != nullptr) {
                        if (registers_G3[register_index].scale < 1) {
                            this->registers_G3[register_index].sensor->publish_state(((float) response.uint32_t * registers_G3[register_index].scale));
                        } else {
                            this->registers_G3[register_index].sensor->publish_state(response.uint32_t * registers_G3[register_index].scale);
                        }
                    }
                    break;
                case 3: // int32_t
                    if (this->registers_G3[register_index].sensor != nullptr) {
                        if (registers_G3[register_index].scale < 1) {
                            this->registers_G3[register_index].sensor->publish_state(((float) response.int32_t * registers_G3[register_index].scale));
                        } else {
                            this->registers_G3[register_index].sensor->publish_state(response.int32_t * registers_G3[register_index].scale);
                        }
                    }
                    break;
                default:
                    ESP_LOGE(TAG, "Unknown register type for index %d", register_index);
            }
        }

        void SofarSolar_Inverter::dump_config(){
            ESP_LOGCONFIG(TAG, "SofarSolar_Inverter");
            ESP_LOGCONFIG(TAG, "  model = %s", this->model_.c_str());
            ESP_LOGCONFIG(TAG, "  modbus_address = %i", this->modbus_address_);
            ESP_LOGCONFIG(TAG, "  zero_export = %s", TRUEFALSE(this->zero_export_));
            ESP_LOGCONFIG(TAG, "  power_sensor = %s", this->power_sensor_ ? this->power_sensor_->get_name().c_str() : "None");
            ESP_LOGCONFIG(TAG, "  pv_generation_today_sensor = %s", this->pv_generation_today_sensor_ ? this->pv_generation_today_sensor_->get_name().c_str() : "Not set");
            ESP_LOGCONFIG(TAG, "      with update interval: %d seconds, default value: %f, enforce default value: %s", registers_G3[0].update_interval, registers_G3[i].is_default_value_set ? registers_G3[0].default_value * registers_G3[0].scale : 0.0, TRUEFALSE(registers_G3[0].enforce_default_value));
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