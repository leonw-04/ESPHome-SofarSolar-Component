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
			uint8_t write_funktion = 0; // Function pointer for writing to the register
            SofarSolar_Register() : start_address(0), quantity(0), type(0), priority(0), scale(0.0f), writeable(false), write_funktion(0) {}
            SofarSolar_Register(uint16_t start_address, uint16_t quantity, uint8_t type, uint8_t priority, float scale, bool writeable, uint8_t write_funktion) : start_address(start_address), quantity(quantity), type(type), priority(priority), scale(scale), writeable(writeable), write_funktion(write_funktion) {}
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

        SofarSolar_Inverter::SofarSolar_Inverter() {
            this->registers_G3 = {
			{PV_GENERATION_TODAY, SofarSolar_Register{0x0684, 2, 2, 1, 0.01, false, 0}}, // PV Generation Today
            {PV_GENERATION_TOTAL, SofarSolar_Register{0x0686, 2, 2, 0, 0.1, false, 0}}, // PV Generation Total
            {LOAD_CONSUMPTION_TODAY, SofarSolar_Register{0x0688, 2, 2, 1, 0.01, false, 0}}, // Load Consumption Today
			{LOAD_CONSUMPTION_TOTAL, SofarSolar_Register{0x068A, 2, 2, 0, 0.1, false, 0}}, // Load Consumption Total
            {BATTERY_CHARGE_TODAY, SofarSolar_Register{0x0694, 2, 2, 1, 0.01, false, 0}}, // Battery Charge Today
            {BATTERY_CHARGE_TOTAL, SofarSolar_Register{0x0696, 2, 2, 0, 0.1, false, 0}}, // Battery Charge Total
            {BATTERY_DISCHARGE_TODAY, SofarSolar_Register{0x0698, 2, 2, 1, 0.01, false, 0}}, // Battery Discharge Today
            {BATTERY_DISCHARGE_TOTAL, SofarSolar_Register{0x069A, 2, 2, 0, 0.1, false, 0}}, // Battery Discharge Total
            {TOTAL_ACTIVE_POWER_INVERTER, SofarSolar_Register{0x0485, 1, 1, 3, 10 ,false, 0}}, // Total Active Power Inverter
            {PV_VOLTAGE_1 ,SofarSolar_Register{0x0584 ,1 ,0 ,2 ,0.1 ,false, 0}}, // PV Voltage 1
            {PV_CURRENT_1 ,SofarSolar_Register{0x0585 ,1 ,0 ,2 ,0.01 ,false, 0}}, // PV Current 1
            {PV_POWER_1 ,SofarSolar_Register{0x0586 ,1 ,0 ,2 ,10 ,false, 0}}, // PV Power 1
            {PV_VOLTAGE_2 ,SofarSolar_Register{0x0588 ,1 ,0 ,2 ,0.1 ,false, 0}}, // PV Voltage 2
            {PV_CURRENT_2 ,SofarSolar_Register{0x0589 ,1 ,0 ,2 ,0.01 ,false, 0}}, // PV Current 2
            {PV_POWER_2 ,SofarSolar_Register{0x058A ,1 ,0 ,2 ,10 ,false, 0}}, // PV Power 2
            {PV_POWER_TOTAL ,SofarSolar_Register{0x05C4 ,1 ,0 ,3 ,100 ,false, 0}}, // PV Power Total
            {BATTERY_POWER_TOTAL ,SofarSolar_Register{0x0667 ,1 ,1 ,3 ,100 ,false, 0}}, // Battery Power Total
            {BATTERY_STATE_OF_CHARGE_TOTAL ,SofarSolar_Register{0x0668 ,1 ,0 ,1 ,1 ,false, 0}}, // Battery State of Charge Total
            {DESIRED_GRID_POWER ,SofarSolar_Register{0x1187 ,2 ,3 ,3 ,1 ,true, DESIRED_GRID_POWER_WRITE}}, // Desired Grid Power
            {MINIMUM_BATTERY_POWER ,SofarSolar_Register{0x1189 ,2 ,3 ,3 ,1 ,true, DESIRED_GRID_POWER_WRITE}}, // Minimum Battery Power
            {MAXIMUM_BATTERY_POWER ,SofarSolar_Register{0x118B ,2 ,3 ,3 ,1 ,true, DESIRED_GRID_POWER_WRITE}}, // Maximum Battery Power
            {ENERGY_STORAGE_MODE, SofarSolar_Register{0x1110, 1, 0, 0, 1, true, SINGLE_REGISTER_WRITE}}, // Energy Storage Mode
            {BATTERY_CONF_ID, SofarSolar_Register{0x1044, 1, 0, 0, 1, true, BATTERY_CONF_WRITE}}, // Battery Conf ID
            {BATTERY_CONF_ADDRESS, SofarSolar_Register{0x1045, 1, 0, 0, 1, true, BATTERY_CONF_WRITE}}, // Battery Conf Address
            {BATTERY_CONF_PROTOCOL, SofarSolar_Register{0x1046, 1, 0, 0, 1, true, BATTERY_CONF_WRITE}}, // Battery Conf Protocol
            {BATTERY_CONF_VOLTAGE_NOMINAL, SofarSolar_Register{0x1050, 1, 0, 0, 0.1, true, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Nominal
            {BATTERY_CONF_VOLTAGE_OVER, SofarSolar_Register{0x1047, 1, 0, 0, 0.1, true, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Over
            {BATTERY_CONF_VOLTAGE_CHARGE, SofarSolar_Register{0x1048, 1, 0, 0, 0.1, true, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Charge
            {BATTERY_CONF_VOLTAGE_LACK,SofarSolar_Register{0x1049 ,1 ,0 ,0 ,0.1 ,true ,BATTERY_CONF_WRITE}}, // Battery Conf Voltage Lack
            {BATTERY_CONF_VOLTAGE_DISCHARGE_STOP, SofarSolar_Register{0x104A, 1, 0, 0, 0.1, true, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Discharge Stop
            {BATTERY_CONF_CURRENT_CHARGE_LIMIT, SofarSolar_Register{0x104B, 1, 0, 0, 0.01, true, BATTERY_CONF_WRITE}}, // Battery Conf Current Charge Limit
            {BATTERY_CONF_CURRENT_DISCHARGE_LIMIT, SofarSolar_Register{0x104C, 1, 0, 0, 0.01, true, BATTERY_CONF_WRITE}}, // Battery Conf Current Discharge Limit
            {BATTERY_CONF_DEPTH_OF_DISCHARGE, SofarSolar_Register{0x104D, 1, 0, 0, 1, true, BATTERY_CONF_WRITE}}, // Battery Conf Depth of Discharge
            {BATTERY_CONF_END_OF_DISCHARGE, SofarSolar_Register{0x104E ,1 ,0 ,0 ,1 ,true ,BATTERY_CONF_WRITE}}, // Battery Conf End of Discharge
            {BATTERY_CONF_CAPACITY, SofarSolar_Register{0x104F ,1 ,0 ,0 ,10 ,true ,BATTERY_CONF_WRITE}}, // Battery Conf Capacity
            {BATTERY_CONF_CELL_TYPE, SofarSolar_Register{0x1051 ,1 ,0 ,0 ,1 ,true ,BATTERY_CONF_WRITE}}, // Battery Conf Cell Type
            {BATTERY_CONF_EPS_BUFFER, SofarSolar_Register{0x1052 ,1 ,0 ,0 ,10 ,true ,BATTERY_CONF_WRITE}}, // Battery Conf EPS Buffer
            {BATTERY_CONF_CONTROL, SofarSolar_Register{0x1053 ,1 ,0 ,0 ,1 ,true ,BATTERY_CONF_WRITE}}, // Battery Conf Control            {GRID_FREQUENCY, SofarSolar_Register{0x0486, 1, 0, 2, 0.01, false}}, // Grid Frequency
            {GRID_VOLTAGE_PHASE_R, SofarSolar_Register{0x0580, 1, 0, 2, 0.1, false, 0}}, // Grid Voltage Phase R
            {GRID_CURRENT_PHASE_R, SofarSolar_Register{0x0581, 1, 0, 2, 0.01, false ,0}}, // Grid Current Phase R
            {GRID_POWER_PHASE_R, SofarSolar_Register{0x0582, 1, 0, 2, 10, false, 0}}, // Grid Power Phase R
            {GRID_VOLTAGE_PHASE_S, SofarSolar_Register{0x058C ,1 ,0 ,2 ,0.1 ,false, 0}}, // Grid Voltage Phase S
            {GRID_CURRENT_PHASE_S, SofarSolar_Register{0x058D ,1 ,0 ,2 ,0.01 ,false, 0}}, // Grid Current Phase S
            {GRID_POWER_PHASE_S, SofarSolar_Register{0x058E ,1 ,0 ,2 ,10 ,false, 0}}, // Grid Power Phase S
            {GRID_VOLTAGE_PHASE_T, SofarSolar_Register{0x0598 ,1 ,0 ,2 ,0.1 ,false, 0}}, // Grid Voltage Phase T
            {GRID_CURRENT_PHASE_T, SofarSolar_Register{0x0599 ,1 ,0 ,2 ,0.01 ,false, 0}}, // Grid Current Phase T
            {GRID_POWER_PHASE_T, SofarSolar_Register{0x059A ,1 ,0 ,2 ,10 ,false, 0}}, // Grid Power Phase T
            {OFF_GRID_POWER_TOTAL, SofarSolar_Register{0x05A4 ,1 ,0 ,3 ,100 ,false, 0}}, // Off Grid Power Total
            {OFF_GRID_FREQUENCY, SofarSolar_Register{0x05A5, 1, 0, 2, 0.01, false, 0}}, // Off Grid Frequency
            {OFF_GRID_VOLTAGE_PHASE_R, SofarSolar_Register{0x05A6 ,1 ,0 ,2 ,0.1 ,false, 0}}, // Off Grid Voltage Phase R
            {OFF_GRID_CURRENT_PHASE_R, SofarSolar_Register{0x05A7 ,1 ,0 ,2 ,0.01 ,false, 0}}, // Off Grid Current Phase R
            {OFF_GRID_POWER_PHASE_R, SofarSolar_Register{0x05A8 ,1 ,0 ,2 ,10 ,false, 0}}, // Off Grid Power Phase R
            {OFF_GRID_VOLTAGE_PHASE_S, SofarSolar_Register{0x05AC ,1 ,0 ,2 ,0.1 ,false, 0}}, // Off Grid Voltage Phase S
            {OFF_GRID_CURRENT_PHASE_S, SofarSolar_Register{0x05AD ,1 ,0 ,2 ,0.01 ,false, 0}}, // Off Grid Current Phase S
            {OFF_GRID_POWER_PHASE_S, SofarSolar_Register{0x05AE ,1 ,0 ,2 ,10 ,false, 0}}, // Off Grid Power Phase S
            {OFF_GRID_VOLTAGE_PHASE_T, SofarSolar_Register{0x05B8 ,1 ,0 ,2 ,0.1 ,false, 0}}, // Off Grid Voltage Phase T
            {OFF_GRID_CURRENT_PHASE_T, SofarSolar_Register{0x05B9, 1, 0, 2, 0.01, false, 0}}, // Off Grid Current Phase T
            {OFF_GRID_POWER_PHASE_T, SofarSolar_Register{0x05BA, 1, 0, 2, 10, false, 0}} // Off Grid Power Phase T
            };
        }

        int time_last_loop = 0;
        bool current_reading = false; // Pointer to the current reading task
        bool current_writing = false; // Pointer to the current writing task
        register_write_task current_write_task; // Pointer to the current writing task
        uint64_t time_begin_modbus_operation = 0;
        uint64_t zero_export_last_update = 0;
        std::priority_queue<register_read_task> register_tasks;

        void SofarSolar_Inverter::setup() {
            // Code here should perform all component initialization,
            //  whether hardware, memory, or otherwise
        }

        void SofarSolar_Inverter::loop() {
            if (!current_reading && !current_writing && this->zero_export_ && millis() - zero_export_last_update > 1000) { // Update zero export every second
                zero_export_last_update = millis();
                ESP_LOGD(TAG, "Updating zero export status");
                // Read the current zero export status
                registers_G3[DESIRED_GRID_POWER].write_value.int32_value = this->total_active_power_inverter_sensor_->state + this->power_sensor_->state;
                registers_G3[MINIMUM_BATTERY_POWER].write_value.int32_value = -5000;
                registers_G3[MINIMUM_BATTERY_POWER].write_value.int32_value = 5000;
                ESP_LOGD(TAG, "Current total active power inverter: %f W, Current power sensor: %f W, New desired grid power: %d W", this->total_active_power_inverter_sensor_->state, this->power_sensor_->state, registers_G3[DESIRED_GRID_POWER].write_value.int32_value);
                register_write_task data;
                data.register_ptr = &registers_G3[DESIRED_GRID_POWER];
                time_begin_modbus_operation = millis();
                this->empty_uart_buffer(); // Clear the UART buffer before sending a new request
                this->write_desired_grid_power(data); // Write the new desired grid power, minimum battery power, and maximum battery power
                current_writing = true; // Set the flag to indicate that a zero export write is in progress
                current_write_task = data; // Set the flag to indicate that a zero export write is in progress
            }
            ESP_LOGVV(TAG, "Elements in register_tasks: %d", register_tasks.size());
            for (auto &pair : registers_G3) {
                if (pair.second.sensor == nullptr) {
                    ESP_LOGVV(TAG, "Sensor for register %d is not set", pair.second.start_address);
                    continue;
                }
                ESP_LOGVV(TAG, "Checking register %04X: Time since last update: %d seconds, Update interval: %d seconds", pair.second.start_address, millis() / 1000 - pair.second.timer, pair.second.update_interval);
                if (millis() - pair.second.timer > pair.second.update_interval * 1000 && !pair.second.is_queued) {
                    pair.second.timer = millis();
                    // Create a task for the register
                    register_read_task task;
                    task.register_ptr = &pair.second;
                    // Add the task to a priority queue
                    register_tasks.push(task);
                    pair.second.is_queued = true;
                    ESP_LOGV(TAG, "Register %04X is queued for reading", pair.second.start_address);
                }
            }

            if (!current_reading && !current_writing && !register_tasks.empty()) {
                // Get the highest priority task
                register_read_task task = register_tasks.top();
                current_reading = true;
                time_begin_modbus_operation = millis();
                empty_uart_buffer(); // Clear the UART buffer before sending a new request
                send_read_modbus_registers(task.register_ptr->start_address, task.register_ptr->quantity);
            } else if (current_reading) {
                if (millis() - time_begin_modbus_operation > 500) { // Timeout after 500 ms
                    ESP_LOGE(TAG, "Timeout while waiting for response");
                    current_reading = false;
                    register_tasks.top().register_ptr->is_queued = false; // Mark the register as not queued anymore
                    register_tasks.pop(); // Remove the task from the queue
                    return;
                }
                std::vector<uint8_t> response;
                if (check_for_response()) {
                    register_read_task task = register_tasks.top(); // Get the current task
                    register_tasks.pop();
                    current_reading = false;
                    if (read_response(response, *task.register_ptr)) {
                        if (extract_data_from_response(response, task)) {
                            ESP_LOGD(TAG, "Read successful for register %04X: Value: %s", task.register_ptr->start_address, vector_to_string(response).c_str());
                            update_sensor(task);
                        } else {
                            ESP_LOGE(TAG, "Failed to extract data from response for register %04X", task.register_ptr->start_address);
                        }
                        if (task.register_ptr->is_default_value_set) {
                            if (task.read_value.uint64_value != task.register_ptr->default_value.uint64_value) { // Use default value if set
                                time_begin_modbus_operation = millis();
                                switch (task.register_ptr->write_funktion) {
                                    case DESIRED_GRID_POWER_WRITE:
                                        ESP_LOGD(TAG, "Writing desired grid power");
                                        task.register_ptr->write_value.int32_value = task.read_value.int32_value;
                                        register_write_task write_task;
                                        write_task.register_ptr = task.register_ptr;
                                        this->write_desired_grid_power(write_task);
                                        current_writing = true; // Set the flag to indicate that a write is in progress
                                        current_write_task = write_task; // Set the current write task
                                        break;
                                    case BATTERY_CONF_WRITE:
                                        ESP_LOGD(TAG, "Writing battery configuration");
                                        task.register_ptr->write_value.uint16_value = task.read_value.uint16_value;
                                        write_task.register_ptr = task.register_ptr;
                                        this->write_battery_conf(write_task);
                                        current_writing = true; // Set the flag to indicate that a write is in progress
                                        current_write_task = write_task; // Set the current write task
                                        break;
                                    case SINGLE_REGISTER_WRITE:
                                        ESP_LOGD(TAG, "Writing single register: %04X", task.register_ptr->start_address);
                                        task.register_ptr->write_value.uint64_value = task.read_value.uint64_value;
                                        write_task.register_ptr = task.register_ptr;
                                        this->write_single_register(write_task);
                                        current_writing = true; // Set the flag to indicate that a write is in progress
                                        current_write_task = write_task; // Set the current write task
                                        break;
                                    default:
                                        ESP_LOGE(TAG, "Unknown write function: %d", task.register_ptr->write_funktion);
                                        break;
                                }
                            }
                        }
                    } else {
                        task.register_ptr->is_queued = false; // Mark the register as not queued anymore
                        ESP_LOGE(TAG, "Invalid response");
                    }
                } else {
                    ESP_LOGE(TAG, "No response received");
                }
            } else if (current_writing) {
                if (millis() - time_begin_modbus_operation > 500) { // Timeout after 500 ms
                    ESP_LOGE(TAG, "Timeout while waiting for write response");
                    current_writing = false;
                    return;
                }
                std::vector<uint8_t> response;
                if (!check_for_response()) {
                    ESP_LOGV(TAG, "No response received write");
                } else {
                    current_writing = false;
                    if (write_response(current_write_task)) {
                        ESP_LOGD(TAG, "Write successful");
                    } else {
                        ESP_LOGE(TAG, "Invalid response for write");
                    }
                }
            }
        }

        bool SofarSolar_Inverter::check_for_response() {
            // Check if there is a response available in the UART buffer
            this->peek(); // Peek to check if there is data available
            if (this->available()) {
                return true; // Not enough bytes for a valid response
            }
            return false;
        }

        bool SofarSolar_Inverter::read_response(std::vector<uint8_t> &response, SofarSolar_Register &register_info) {
            // Read the response from the UART buffer
            response.clear();
            while (this->available()) {
                uint8_t byte = this->read();
                response.push_back(byte);
            }
            ESP_LOGVV(TAG, "Received response: %s", vector_to_string(response).c_str());
            if (!check_crc(response)) { // Check CRC after reading the response
                return false; // Invalid CRC
            }
            if (!check_for_error_code(response)) {
                return false; // Error code present in the response
            }
            if (response.data()[1] != 0x03) {
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

        bool SofarSolar_Inverter::extract_data_from_response(std::vector<uint8_t> &response, register_read_task &task) {
            switch (response.data()[2]) {
                case 2:
                    task.read_value.uint16_t = ((response.data()[3] << 8) | response.data()[4]); // 2 bytes for start address
                    break;
                case 4:
                    task.read_value.uint32_t = ((response.data()[3] << 24) | (response.data()[4] << 16) | (response.data()[5] << 8) | response.data()[6]); // 4 bytes for start address
                    break;
                case 8:
                    task.read_value.uint64_t = ((response.data()[3] << 56) | (response.data()[4] << 48) | (response.data()[5] << 40) | (response.data()[6] << 32) |
                           (response.data()[7] << 24) | (response.data()[8] << 16) | (response.data()[9] << 8) | response.data()[10]); // 8 bytes for start address
                    break;
                default:
                    ESP_LOGE(TAG, "Invalid response size for extracting value");
                    return false; // Invalid response size
            }
            return true; // Valid data extraction
        }

        bool SofarSolar_Inverter::write_response(register_write_task &task) {
            // Read the response from the UART buffer
            std::vector<uint8_t> response;
            response.clear();
            while (this->available()) {
                uint8_t byte = this->read();
                response.push_back(byte);
            }
            if (!check_crc(response)) { // Check CRC after reading the response
                return false; // Invalid CRC
            }
            if (!check_for_error_code(response)) {
                return false; // Error code present in the response
            }
            if (response.data()[1] != 0x10) {
                ESP_LOGE(TAG, "Invalid Modbus response function code: %02X", response.data()[1]);
                response.clear(); // Clear the response on invalid function code
                return false; // Invalid function code
            }
            if (task.register_ptr->start_address != ((response.data()[2] << 8) | (response.data()[3]))) {
                ESP_LOGE(TAG, "Invalid response address: expected %04X, got %02X%02X", task.register_ptr->start_address, response.data()[2], response.data()[3]);
                return false; // Invalid response size
            }
            if (task.number_of_registers != ((response.data()[4] << 8) | (response.data()[5]))) {
                ESP_LOGE(TAG, "Invalid response quantity: expected %d, got %02X", task.number_of_registers, ((response.data()[4] << 8) | (response.data()[5])));
                return false; // Invalid response size
            }
            return true; // Valid write response
        }

        bool SofarSolar_Inverter::check_for_error_code(std::vector<uint8_t> &response) {
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
            task.number_of_registers = (data.size() >> 1); // Number of registers to write
            send_write_modbus_registers(registers_G3[DESIRED_GRID_POWER].start_address, task.number_of_registers, data);
        }

        void SofarSolar_Inverter::write_battery_conf(register_write_task &task) {
            // Write the battery configuration
            std::vector<uint8_t> data;
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
                new_battery_conf_protocol = registers_G3[BATTERY_CONF_PROTOCOL].default_value.uint16_value;
            } else if (registers_G3[BATTERY_CONF_PROTOCOL].write_set_value) {
                new_battery_conf_protocol = registers_G3[BATTERY_CONF_PROTOCOL].write_value.uint16_value;
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
            task.number_of_registers = (data.size() >> 1); // Number of registers to write
            send_write_modbus_registers(registers_G3[BATTERY_CONF_ID].start_address, task.number_of_registers, data);
        }

        void SofarSolar_Inverter::write_single_register(register_write_task &task) {
            SofarSolar_RegisterValue value;
            if (task.register_ptr->enforce_default_value && task.register_ptr->is_default_value_set) {
                value = task.register_ptr->default_value; // Use default value if set
            } else if (task.register_ptr->write_set_value) {
                value = task.register_ptr->write_value; // Use write value if set
            } else {
                ESP_LOGE(TAG, "No value set for writing to register %04X", task.register_ptr->start_address);
                return; // Exit if no value is set
            }
            std::vector<uint8_t> data;
            switch (task.register_ptr->type) {
                case 0: // uint16_t
                    data.push_back(static_cast<uint8_t>(value.uint16_value >> 8));
                    data.push_back(static_cast<uint8_t>(value.uint16_value & 0xFF));
                    break;
                case 1: // int16_t
                    data.push_back(static_cast<uint8_t>(value.int16_value >> 8));
                    data.push_back(static_cast<uint8_t>(value.int16_value & 0xFF));
                    break;
                case 2: // uint32_t
                    data.push_back(static_cast<uint8_t>(value.uint32_value >> 24));
                    data.push_back(static_cast<uint8_t>(value.uint32_value >> 16));
                    data.push_back(static_cast<uint8_t>(value.uint32_value >> 8));
                    data.push_back(static_cast<uint8_t>(value.uint32_value & 0xFF));
                    break;
                case 3: // int32_t
                    data.push_back(static_cast<uint8_t>(value.int32_value >> 24));
                    data.push_back(static_cast<uint8_t>(value.int32_value >> 16));
                    data.push_back(static_cast<uint8_t>(value.int32_value >> 8));
                    data.push_back(static_cast<uint8_t>(value.int32_value & 0xFF));
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
                    ESP_LOGE(TAG, "Unknown register type for writing: %d", task.register_ptr->type);
                    return; // Exit if the register type is unknown
            }
            task.number_of_registers = (data.size() >> 1); // Number of registers to write
            send_write_modbus_registers(task.register_ptr->start_address, task.number_of_registers, data);
        }

        void SofarSolar_Inverter::update_sensor(const register_read_task task) {
            // Update the sensor based on the register index and response data
            switch (task.register_ptr->type) {
                case 0: // uint16_t
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.uint16_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.uint16_value * task.register_ptr->scale);
                        }

                    }
                    break;
                case 1: // int16_t
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.int16_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.int16_value * task.register_ptr->scale);
                        }
                    }
                    break;
                case 2: // uint32_t
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.uint32_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.uint32_value * task.register_ptr->scale);
                        }
                    }
                    break;
                case 3: // int32_t
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.int32_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.int32_value * task.register_ptr->scale);
                        }
                    }
                    break;
                case 4: // uint64_t
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.uint64_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.uint64_value * task.register_ptr->scale);
                        }
                    }
                    break;
                case 5: // int64_t
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.int64_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.int64_value * task.register_ptr->scale);
                        }
                    }
                    break;
                case 6: // float
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.float_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.float_value * task.register_ptr->scale);
                        }
                    }
                    break;
                case 7: // double
                    if (task.register_ptr->sensor != nullptr) {
                        if (task.register_ptr->scale < 1) {
                            task.register_ptr->sensor->publish_state(((float) task.read_value.double_value * task.register_ptr->scale));
                        } else {
                            task.register_ptr->sensor->publish_state(task.read_value.double_value * task.register_ptr->scale);
                        }
                    }
                    break;
                default:
                    ESP_LOGE(TAG, "Unknown register type for updating sensor: %d", task.register_ptr->type);
                    return; // Exit if the register type is unknown
            }
        }

        void SofarSolar_Inverter::dump_config(){
            ESP_LOGCONFIG(TAG, "SofarSolar_Inverter");
            ESP_LOGCONFIG(TAG, "  model = %s", this->model_.c_str());
            ESP_LOGCONFIG(TAG, "  modbus_address = %i", this->modbus_address_);
            ESP_LOGCONFIG(TAG, "  zero_export = %s", TRUEFALSE(this->zero_export_));
            ESP_LOGCONFIG(TAG, "  power_sensor = %s", this->power_sensor_ ? this->power_sensor_->get_name().c_str() : "None");
            ESP_LOGCONFIG(TAG, "  pv_generation_today_sensor = %s", this->pv_generation_today_sensor_ ? this->pv_generation_today_sensor_->get_name().c_str() : "Not set");
            ESP_LOGCONFIG(TAG, "      with update interval: %d seconds, default value: %f, enforce default value: %s", registers_G3[PV_GENERATION_TODAY].update_interval, registers_G3[PV_GENERATION_TODAY].is_default_value_set ? registers_G3[PV_GENERATION_TODAY].default_value.int64_value * registers_G3[PV_GENERATION_TODAY].scale : 0.0, TRUEFALSE(registers_G3[PV_GENERATION_TODAY].enforce_default_value));
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
        void SofarSolar_Inverter::set_pv_generation_today_sensor(sensor::Sensor *pv_generation_today_sensor) { registers_G3[PV_GENERATION_TODAY].sensor = pv_generation_today_sensor; pv_generation_today_sensor_ = pv_generation_today_sensor; }
        void SofarSolar_Inverter::set_pv_generation_total_sensor(sensor::Sensor *pv_generation_total_sensor) { registers_G3[PV_GENERATION_TOTAL].sensor = pv_generation_total_sensor; pv_generation_total_sensor_ = pv_generation_total_sensor; }
        void SofarSolar_Inverter::set_load_consumption_today_sensor(sensor::Sensor *load_consumption_today_sensor) { registers_G3[LOAD_CONSUMPTION_TODAY].sensor = load_consumption_today_sensor; load_consumption_today_sensor_ = load_consumption_today_sensor; }
        void SofarSolar_Inverter::set_load_consumption_total_sensor(sensor::Sensor *load_consumption_total_sensor) { registers_G3[LOAD_CONSUMPTION_TOTAL].sensor = load_consumption_total_sensor; load_consumption_total_sensor_ = load_consumption_total_sensor; }
        void SofarSolar_Inverter::set_battery_charge_today_sensor(sensor::Sensor *battery_charge_today_sensor) { registers_G3[BATTERY_CHARGE_TODAY].sensor = battery_charge_today_sensor; battery_charge_today_sensor_ = battery_charge_today_sensor; }
        void SofarSolar_Inverter::set_battery_charge_total_sensor(sensor::Sensor *battery_charge_total_sensor) { registers_G3[BATTERY_CHARGE_TOTAL].sensor = battery_charge_total_sensor; battery_charge_total_sensor_ = battery_charge_total_sensor; }
        void SofarSolar_Inverter::set_battery_discharge_today_sensor(sensor::Sensor *battery_discharge_today_sensor) { registers_G3[BATTERY_DISCHARGE_TODAY].sensor = battery_discharge_today_sensor; battery_discharge_today_sensor_ = battery_discharge_today_sensor; }
        void SofarSolar_Inverter::set_battery_discharge_total_sensor(sensor::Sensor *battery_discharge_total_sensor) { registers_G3[BATTERY_DISCHARGE_TOTAL].sensor = battery_discharge_total_sensor; battery_discharge_total_sensor_ = battery_discharge_total_sensor; }
        void SofarSolar_Inverter::set_total_active_power_inverter_sensor(sensor::Sensor *total_active_power_inverter_sensor) { registers_G3[TOTAL_ACTIVE_POWER_INVERTER].sensor = total_active_power_inverter_sensor; total_active_power_inverter_sensor_ = total_active_power_inverter_sensor; }
        void SofarSolar_Inverter::set_pv_voltage_1_sensor(sensor::Sensor *pv_voltage_1_sensor) { registers_G3[PV_VOLTAGE_1].sensor = pv_voltage_1_sensor; pv_voltage_1_sensor_ = pv_voltage_1_sensor; }
        void SofarSolar_Inverter::set_pv_current_1_sensor(sensor::Sensor *pv_current_1_sensor) { registers_G3[PV_CURRENT_1].sensor = pv_current_1_sensor; pv_current_1_sensor_ = pv_current_1_sensor; }
        void SofarSolar_Inverter::set_pv_power_1_sensor(sensor::Sensor *pv_power_1_sensor) { registers_G3[PV_POWER_1].sensor = pv_power_1_sensor; pv_power_1_sensor_ = pv_power_1_sensor; }
        void SofarSolar_Inverter::set_pv_voltage_2_sensor(sensor::Sensor *pv_voltage_2_sensor) { registers_G3[PV_VOLTAGE_2].sensor = pv_voltage_2_sensor; pv_voltage_2_sensor_ = pv_voltage_2_sensor; }
        void SofarSolar_Inverter::set_pv_current_2_sensor(sensor::Sensor *pv_current_2_sensor) { registers_G3[PV_CURRENT_2].sensor = pv_current_2_sensor; pv_current_2_sensor_ = pv_current_2_sensor; }
        void SofarSolar_Inverter::set_pv_power_2_sensor(sensor::Sensor *pv_power_2_sensor) { registers_G3[PV_POWER_2].sensor = pv_power_2_sensor; pv_power_2_sensor_ = pv_power_2_sensor; }
        void SofarSolar_Inverter::set_pv_power_total_sensor(sensor::Sensor *pv_power_total_sensor) { registers_G3[PV_POWER_TOTAL].sensor = pv_power_total_sensor; pv_power_total_sensor_ = pv_power_total_sensor; }
        void SofarSolar_Inverter::set_battery_power_total_sensor(sensor::Sensor *battery_power_total_sensor) { registers_G3[BATTERY_POWER_TOTAL].sensor = battery_power_total_sensor; battery_power_total_sensor_ = battery_power_total_sensor; }
        void SofarSolar_Inverter::set_battery_state_of_charge_total_sensor(sensor::Sensor *battery_state_of_charge_total_sensor) { registers_G3[BATTERY_STATE_OF_CHARGE_TOTAL].sensor = battery_state_of_charge_total_sensor; battery_state_of_charge_total_sensor_ = battery_state_of_charge_total_sensor; }
        void SofarSolar_Inverter::set_desired_grid_power_sensor(sensor::Sensor *desired_grid_power_sensor) { registers_G3[DESIRED_GRID_POWER].sensor = desired_grid_power_sensor; desired_grid_power_sensor_ = desired_grid_power_sensor; }
        void SofarSolar_Inverter::set_minimum_battery_power_sensor(sensor::Sensor *minimum_battery_power_sensor) { registers_G3[MINIMUM_BATTERY_POWER].sensor = minimum_battery_power_sensor; minimum_battery_power_sensor_ = minimum_battery_power_sensor; }
        void SofarSolar_Inverter::set_maximum_battery_power_sensor(sensor::Sensor *maximum_battery_power_sensor) { registers_G3[MAXIMUM_BATTERY_POWER].sensor = maximum_battery_power_sensor; maximum_battery_power_sensor_ = maximum_battery_power_sensor; }
        void SofarSolar_Inverter::set_energy_storage_mode_sensor(sensor::Sensor *energy_storage_mode_sensor) { registers_G3[ENERGY_STORAGE_MODE].sensor = energy_storage_mode_sensor; energy_storage_mode_sensor_ = energy_storage_mode_sensor; }
        void SofarSolar_Inverter::set_battery_conf_id_sensor(sensor::Sensor *battery_conf_id_sensor) { registers_G3[BATTERY_CONF_ID].sensor = battery_conf_id_sensor; battery_conf_id_sensor_ = battery_conf_id_sensor; }
        void SofarSolar_Inverter::set_battery_conf_address_sensor(sensor::Sensor *battery_conf_address_sensor) { registers_G3[BATTERY_CONF_ADDRESS].sensor = battery_conf_address_sensor; battery_conf_address_sensor_ = battery_conf_address_sensor; }
        void SofarSolar_Inverter::set_battery_conf_protocol_sensor(sensor::Sensor *battery_conf_protocol_sensor) { registers_G3[BATTERY_CONF_PROTOCOL].sensor = battery_conf_protocol_sensor; battery_conf_protocol_sensor_ = battery_conf_protocol_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor(sensor::Sensor *battery_conf_voltage_over_sensor) { registers_G3[BATTERY_CONF_VOLTAGE_OVER].sensor = battery_conf_voltage_over_sensor; battery_conf_voltage_over_sensor_ = battery_conf_voltage_over_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor(sensor::Sensor *battery_conf_voltage_charge_sensor) { registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].sensor = battery_conf_voltage_charge_sensor; battery_conf_voltage_charge_sensor_ = battery_conf_voltage_charge_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor(sensor::Sensor *battery_conf_voltage_lack_sensor) { registers_G3[BATTERY_CONF_VOLTAGE_LACK].sensor = battery_conf_voltage_lack_sensor; battery_conf_voltage_lack_sensor_ = battery_conf_voltage_lack_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor(sensor::Sensor *battery_conf_voltage_discharge_stop_sensor) { registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].sensor = battery_conf_voltage_discharge_stop_sensor; battery_conf_voltage_discharge_stop_sensor_ = battery_conf_voltage_discharge_stop_sensor; }
        void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor(sensor::Sensor *battery_conf_current_charge_limit_sensor) { registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].sensor = battery_conf_current_charge_limit_sensor; battery_conf_current_charge_limit_sensor_ = battery_conf_current_charge_limit_sensor; }
        void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor(sensor::Sensor *battery_conf_current_discharge_limit_sensor) { registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].sensor = battery_conf_current_discharge_limit_sensor; battery_conf_current_discharge_limit_sensor_ = battery_conf_current_discharge_limit_sensor; }
        void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor(sensor::Sensor *battery_conf_depth_of_discharge_sensor) { registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].sensor = battery_conf_depth_of_discharge_sensor; battery_conf_depth_of_discharge_sensor_ = battery_conf_depth_of_discharge_sensor; }
        void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor(sensor::Sensor *battery_conf_end_of_discharge_sensor) { registers_G3[BATTERY_CONF_END_OF_DISCHARGE].sensor = battery_conf_end_of_discharge_sensor; battery_conf_end_of_discharge_sensor_ = battery_conf_end_of_discharge_sensor; }
        void SofarSolar_Inverter::set_battery_conf_capacity_sensor(sensor::Sensor *battery_conf_capacity_sensor) { registers_G3[BATTERY_CONF_CAPACITY].sensor = battery_conf_capacity_sensor; battery_conf_capacity_sensor_ = battery_conf_capacity_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor(sensor::Sensor *battery_conf_voltage_nominal_sensor) { registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].sensor = battery_conf_voltage_nominal_sensor; battery_conf_voltage_nominal_sensor_ = battery_conf_voltage_nominal_sensor; }
        void SofarSolar_Inverter::set_battery_conf_cell_type_sensor(sensor::Sensor *battery_conf_cell_type_sensor) { registers_G3[BATTERY_CONF_CELL_TYPE].sensor = battery_conf_cell_type_sensor; battery_conf_cell_type_sensor_ = battery_conf_cell_type_sensor; }
        void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor(sensor::Sensor *battery_conf_eps_buffer_sensor) { registers_G3[BATTERY_CONF_EPS_BUFFER].sensor = battery_conf_eps_buffer_sensor; battery_conf_eps_buffer_sensor_ = battery_conf_eps_buffer_sensor; }
        void SofarSolar_Inverter::set_battery_conf_control_sensor(sensor::Sensor *battery_conf_control_sensor) { registers_G3[BATTERY_CONF_CONTROL].sensor = battery_conf_control_sensor; battery_conf_control_sensor_ = battery_conf_control_sensor; }
        void SofarSolar_Inverter::set_grid_frequency_sensor(sensor::Sensor *grid_frequency_sensor) { registers_G3[GRID_FREQUENCY].sensor = grid_frequency_sensor; grid_frequency_sensor_ = grid_frequency_sensor; }
        void SofarSolar_Inverter::set_grid_voltage_phase_r_sensor(sensor::Sensor *grid_voltage_phase_r_sensor) { registers_G3[GRID_VOLTAGE_PHASE_R].sensor = grid_voltage_phase_r_sensor; grid_voltage_phase_r_sensor_ = grid_voltage_phase_r_sensor; }
        void SofarSolar_Inverter::set_grid_current_phase_r_sensor(sensor::Sensor *grid_current_phase_r_sensor) { registers_G3[GRID_CURRENT_PHASE_R].sensor = grid_current_phase_r_sensor; grid_current_phase_r_sensor_ = grid_current_phase_r_sensor; }
        void SofarSolar_Inverter::set_grid_power_phase_r_sensor(sensor::Sensor *grid_power_phase_r_sensor) { registers_G3[GRID_POWER_PHASE_R].sensor = grid_power_phase_r_sensor; grid_power_phase_r_sensor_ = grid_power_phase_r_sensor; }
        void SofarSolar_Inverter::set_grid_voltage_phase_s_sensor(sensor::Sensor *grid_voltage_phase_s_sensor) { registers_G3[GRID_VOLTAGE_PHASE_S].sensor = grid_voltage_phase_s_sensor; grid_voltage_phase_s_sensor_ = grid_voltage_phase_s_sensor; }
        void SofarSolar_Inverter::set_grid_current_phase_s_sensor(sensor::Sensor *grid_current_phase_s_sensor) { registers_G3[GRID_CURRENT_PHASE_S].sensor = grid_current_phase_s_sensor; grid_current_phase_s_sensor_ = grid_current_phase_s_sensor; }
        void SofarSolar_Inverter::set_grid_power_phase_s_sensor(sensor::Sensor *grid_power_phase_s_sensor) { registers_G3[GRID_POWER_PHASE_S].sensor = grid_power_phase_s_sensor; grid_power_phase_s_sensor_ = grid_power_phase_s_sensor; }
        void SofarSolar_Inverter::set_grid_voltage_phase_t_sensor(sensor::Sensor *grid_voltage_phase_t_sensor) { registers_G3[GRID_VOLTAGE_PHASE_T].sensor = grid_voltage_phase_t_sensor; grid_voltage_phase_t_sensor_ = grid_voltage_phase_t_sensor; }
        void SofarSolar_Inverter::set_grid_current_phase_t_sensor(sensor::Sensor *grid_current_phase_t_sensor) { registers_G3[GRID_CURRENT_PHASE_T].sensor = grid_current_phase_t_sensor; grid_current_phase_t_sensor_ = grid_current_phase_t_sensor; }
        void SofarSolar_Inverter::set_grid_power_phase_t_sensor(sensor::Sensor *grid_power_phase_t_sensor) { registers_G3[GRID_POWER_PHASE_T].sensor = grid_power_phase_t_sensor; grid_power_phase_t_sensor_ = grid_power_phase_t_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_total_sensor(sensor::Sensor *off_grid_power_total_sensor) { registers_G3[OFF_GRID_POWER_TOTAL].sensor = off_grid_power_total_sensor; off_grid_power_total_sensor_ = off_grid_power_total_sensor; }
        void SofarSolar_Inverter::set_off_grid_frequency_sensor(sensor::Sensor *off_grid_frequency_sensor) { registers_G3[OFF_GRID_FREQUENCY].sensor = off_grid_frequency_sensor; off_grid_frequency_sensor_ = off_grid_frequency_sensor; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_r_sensor(sensor::Sensor *off_grid_voltage_phase_r_sensor) { registers_G3[OFF_GRID_VOLTAGE_PHASE_R].sensor = off_grid_voltage_phase_r_sensor; off_grid_voltage_phase_r_sensor_ = off_grid_voltage_phase_r_sensor; }
        void SofarSolar_Inverter::set_off_grid_current_phase_r_sensor(sensor::Sensor *off_grid_current_phase_r_sensor) { registers_G3[OFF_GRID_CURRENT_PHASE_R].sensor = off_grid_current_phase_r_sensor; off_grid_current_phase_r_sensor_ = off_grid_current_phase_r_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_phase_r_sensor(sensor::Sensor *off_grid_power_phase_r_sensor) { registers_G3[OFF_GRID_POWER_PHASE_R].sensor = off_grid_power_phase_r_sensor; off_grid_power_phase_r_sensor_ = off_grid_power_phase_r_sensor; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_s_sensor(sensor::Sensor *off_grid_voltage_phase_s_sensor) { registers_G3[OFF_GRID_VOLTAGE_PHASE_S].sensor = off_grid_voltage_phase_s_sensor; off_grid_voltage_phase_s_sensor_ = off_grid_voltage_phase_s_sensor; }
        void SofarSolar_Inverter::set_off_grid_current_phase_s_sensor(sensor::Sensor *off_grid_current_phase_s_sensor) { registers_G3[OFF_GRID_CURRENT_PHASE_S].sensor = off_grid_current_phase_s_sensor; off_grid_current_phase_s_sensor_ = off_grid_current_phase_s_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_phase_s_sensor(sensor::Sensor *off_grid_power_phase_s_sensor) { registers_G3[OFF_GRID_POWER_PHASE_S].sensor = off_grid_power_phase_s_sensor; off_grid_power_phase_s_sensor_ = off_grid_power_phase_s_sensor; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_t_sensor(sensor::Sensor *off_grid_voltage_phase_t_sensor) { registers_G3[OFF_GRID_VOLTAGE_PHASE_T].sensor = off_grid_voltage_phase_t_sensor; off_grid_voltage_phase_t_sensor_ = off_grid_voltage_phase_t_sensor; }
        void SofarSolar_Inverter::set_off_grid_current_phase_t_sensor(sensor::Sensor *off_grid_current_phase_t_sensor) { registers_G3[OFF_GRID_CURRENT_PHASE_T].sensor = off_grid_current_phase_t_sensor; off_grid_current_phase_t_sensor_ = off_grid_current_phase_t_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_phase_t_sensor(sensor::Sensor *off_grid_power_phase_t_sensor) { registers_G3[OFF_GRID_POWER_PHASE_T].sensor = off_grid_power_phase_t_sensor; off_grid_power_phase_t_sensor_ = off_grid_power_phase_t_sensor; }

        // Set update intervals for sensors

        void SofarSolar_Inverter::set_pv_generation_today_sensor_update_interval(int pv_generation_today_sensor_update_interval) { registers_G3[PV_GENERATION_TODAY].update_interval = pv_generation_today_sensor_update_interval; pv_generation_today_sensor_update_interval_ = pv_generation_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_generation_total_sensor_update_interval(int pv_generation_total_sensor_update_interval) { registers_G3[PV_GENERATION_TOTAL].update_interval = pv_generation_total_sensor_update_interval; pv_generation_total_sensor_update_interval_ = pv_generation_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_load_consumption_today_sensor_update_interval(int load_consumption_today_sensor_update_interval) { registers_G3[LOAD_CONSUMPTION_TODAY].update_interval = load_consumption_today_sensor_update_interval; load_consumption_today_sensor_update_interval_ = load_consumption_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_load_consumption_total_sensor_update_interval(int load_consumption_total_sensor_update_interval) { registers_G3[LOAD_CONSUMPTION_TOTAL].update_interval = load_consumption_total_sensor_update_interval; load_consumption_total_sensor_update_interval_ = load_consumption_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_charge_today_sensor_update_interval(int battery_charge_today_sensor_update_interval) { registers_G3[BATTERY_CHARGE_TODAY].update_interval = battery_charge_today_sensor_update_interval; battery_charge_today_sensor_update_interval_ = battery_charge_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_charge_total_sensor_update_interval(int battery_charge_total_sensor_update_interval) { registers_G3[BATTERY_CHARGE_TOTAL].update_interval = battery_charge_total_sensor_update_interval; battery_charge_total_sensor_update_interval_ = battery_charge_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_discharge_today_sensor_update_interval(int battery_discharge_today_sensor_update_interval) { registers_G3[BATTERY_DISCHARGE_TODAY].update_interval = battery_discharge_today_sensor_update_interval; battery_discharge_today_sensor_update_interval_ = battery_discharge_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_discharge_total_sensor_update_interval(int battery_discharge_total_sensor_update_interval) { registers_G3[BATTERY_DISCHARGE_TOTAL].update_interval = battery_discharge_total_sensor_update_interval; battery_discharge_total_sensor_update_interval_ = battery_discharge_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_total_active_power_inverter_sensor_update_interval(int total_active_power_inverter_sensor_update_interval) { registers_G3[TOTAL_ACTIVE_POWER_INVERTER].update_interval = total_active_power_inverter_sensor_update_interval; total_active_power_inverter_sensor_update_interval_ = total_active_power_inverter_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_voltage_1_sensor_update_interval(int pv_voltage_1_sensor_update_interval) { registers_G3[PV_VOLTAGE_1].update_interval = pv_voltage_1_sensor_update_interval; pv_voltage_1_sensor_update_interval_ = pv_voltage_1_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_current_1_sensor_update_interval(int pv_current_1_sensor_update_interval) { registers_G3[PV_CURRENT_1].update_interval = pv_current_1_sensor_update_interval; pv_current_1_sensor_update_interval_ = pv_current_1_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_power_1_sensor_update_interval(int pv_power_1_sensor_update_interval) { registers_G3[PV_POWER_1].update_interval = pv_power_1_sensor_update_interval; pv_power_1_sensor_update_interval_ = pv_power_1_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_voltage_2_sensor_update_interval(int pv_voltage_2_sensor_update_interval) { registers_G3[PV_VOLTAGE_2].update_interval = pv_voltage_2_sensor_update_interval; pv_voltage_2_sensor_update_interval_ = pv_voltage_2_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_current_2_sensor_update_interval(int pv_current_2_sensor_update_interval) { registers_G3[PV_CURRENT_2].update_interval = pv_current_2_sensor_update_interval; pv_current_2_sensor_update_interval_ = pv_current_2_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_power_2_sensor_update_interval(int pv_power_2_sensor_update_interval) { registers_G3[PV_POWER_2].update_interval = pv_power_2_sensor_update_interval; pv_power_2_sensor_update_interval_ = pv_power_2_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_power_total_sensor_update_interval(int pv_power_total_sensor_update_interval) { registers_G3[PV_POWER_TOTAL].update_interval = pv_power_total_sensor_update_interval; pv_power_total_sensor_update_interval_ = pv_power_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_power_total_sensor_update_interval(int battery_power_total_sensor_update_interval) { registers_G3[BATTERY_POWER_TOTAL].update_interval = battery_power_total_sensor_update_interval; battery_power_total_sensor_update_interval_ = battery_power_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_state_of_charge_total_sensor_update_interval(int battery_state_of_charge_total_sensor_update_interval) { registers_G3[BATTERY_STATE_OF_CHARGE_TOTAL].update_interval = battery_state_of_charge_total_sensor_update_interval; battery_state_of_charge_total_sensor_update_interval_ = battery_state_of_charge_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_desired_grid_power_sensor_update_interval(int desired_grid_power_sensor_update_interval) { registers_G3[DESIRED_GRID_POWER].update_interval = desired_grid_power_sensor_update_interval; desired_grid_power_sensor_update_interval_ = desired_grid_power_sensor_update_interval; }
        void SofarSolar_Inverter::set_minimum_battery_power_sensor_update_interval(int minimum_battery_power_sensor_update_interval) { registers_G3[MINIMUM_BATTERY_POWER].update_interval = minimum_battery_power_sensor_update_interval; minimum_battery_power_sensor_update_interval_ = minimum_battery_power_sensor_update_interval; }
        void SofarSolar_Inverter::set_maximum_battery_power_sensor_update_interval(int maximum_battery_power_sensor_update_interval) { registers_G3[MAXIMUM_BATTERY_POWER].update_interval = maximum_battery_power_sensor_update_interval; maximum_battery_power_sensor_update_interval_ = maximum_battery_power_sensor_update_interval; }
        void SofarSolar_Inverter::set_energy_storage_mode_sensor_update_interval(int energy_storage_mode_sensor_update_interval) { registers_G3[ENERGY_STORAGE_MODE].update_interval = energy_storage_mode_sensor_update_interval; energy_storage_mode_sensor_update_interval_ = energy_storage_mode_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_id_sensor_update_interval(int battery_conf_id_sensor_update_interval) { registers_G3[BATTERY_CONF_ID].update_interval = battery_conf_id_sensor_update_interval; battery_conf_id_sensor_update_interval_ = battery_conf_id_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_address_sensor_update_interval(int battery_conf_address_sensor_update_interval) { registers_G3[BATTERY_CONF_ADDRESS].update_interval = battery_conf_address_sensor_update_interval; battery_conf_address_sensor_update_interval_ = battery_conf_address_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_protocol_sensor_update_interval(int battery_conf_protocol_sensor_update_interval) { registers_G3[BATTERY_CONF_PROTOCOL].update_interval = battery_conf_protocol_sensor_update_interval; battery_conf_protocol_sensor_update_interval_ = battery_conf_protocol_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_update_interval(int battery_conf_voltage_nominal_sensor_update_interval) { registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].update_interval = battery_conf_voltage_nominal_sensor_update_interval; battery_conf_voltage_nominal_sensor_update_interval_ = battery_conf_voltage_nominal_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_update_interval(int battery_conf_voltage_over_sensor_update_interval) { registers_G3[BATTERY_CONF_VOLTAGE_OVER].update_interval = battery_conf_voltage_over_sensor_update_interval; battery_conf_voltage_over_sensor_update_interval_ = battery_conf_voltage_over_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_update_interval(int battery_conf_voltage_charge_sensor_update_interval) { registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].update_interval = battery_conf_voltage_charge_sensor_update_interval; battery_conf_voltage_charge_sensor_update_interval_ = battery_conf_voltage_charge_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_update_interval(int battery_conf_voltage_lack_sensor_update_interval) { registers_G3[BATTERY_CONF_VOLTAGE_LACK].update_interval = battery_conf_voltage_lack_sensor_update_interval; battery_conf_voltage_lack_sensor_update_interval_ = battery_conf_voltage_lack_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_update_interval(int battery_conf_voltage_discharge_stop_sensor_update_interval) { registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].update_interval = battery_conf_voltage_discharge_stop_sensor_update_interval; battery_conf_voltage_discharge_stop_sensor_update_interval_ = battery_conf_voltage_discharge_stop_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_update_interval(int battery_conf_current_charge_limit_sensor_update_interval) { registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].update_interval = battery_conf_current_charge_limit_sensor_update_interval; battery_conf_current_charge_limit_sensor_update_interval_ = battery_conf_current_charge_limit_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_update_interval(int battery_conf_current_discharge_limit_sensor_update_interval) { registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].update_interval = battery_conf_current_discharge_limit_sensor_update_interval; battery_conf_current_discharge_limit_sensor_update_interval_ = battery_conf_current_discharge_limit_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_update_interval(int battery_conf_depth_of_discharge_sensor_update_interval) { registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].update_interval = battery_conf_depth_of_discharge_sensor_update_interval; battery_conf_depth_of_discharge_sensor_update_interval_ = battery_conf_depth_of_discharge_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_update_interval(int battery_conf_end_of_discharge_sensor_update_interval) { registers_G3[BATTERY_CONF_END_OF_DISCHARGE].update_interval = battery_conf_end_of_discharge_sensor_update_interval; battery_conf_end_of_discharge_sensor_update_interval_ = battery_conf_end_of_discharge_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_capacity_sensor_update_interval(int battery_conf_capacity_sensor_update_interval) { registers_G3[BATTERY_CONF_CAPACITY].update_interval = battery_conf_capacity_sensor_update_interval; battery_conf_capacity_sensor_update_interval_ = battery_conf_capacity_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_update_interval(int battery_conf_cell_type_sensor_update_interval) { registers_G3[BATTERY_CONF_CELL_TYPE].update_interval = battery_conf_cell_type_sensor_update_interval; battery_conf_cell_type_sensor_update_interval_ = battery_conf_cell_type_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_update_interval(int battery_conf_eps_buffer_sensor_update_interval) { registers_G3[BATTERY_CONF_EPS_BUFFER].update_interval = battery_conf_eps_buffer_sensor_update_interval; battery_conf_eps_buffer_sensor_update_interval_ = battery_conf_eps_buffer_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_control_sensor_update_interval(int battery_conf_control_sensor_update_interval) { registers_G3[BATTERY_CONF_CONTROL].update_interval = battery_conf_control_sensor_update_interval; battery_conf_control_sensor_update_interval_ = battery_conf_control_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_frequency_sensor_update_interval(int grid_frequency_sensor_update_interval) { registers_G3[GRID_FREQUENCY].update_interval = grid_frequency_sensor_update_interval; grid_frequency_sensor_update_interval_ = grid_frequency_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_voltage_phase_r_sensor_update_interval(int grid_voltage_phase_r_sensor_update_interval) { registers_G3[GRID_VOLTAGE_PHASE_R].update_interval = grid_voltage_phase_r_sensor_update_interval; grid_voltage_phase_r_sensor_update_interval_ = grid_voltage_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_current_phase_r_sensor_update_interval(int grid_current_phase_r_sensor_update_interval) { registers_G3[GRID_CURRENT_PHASE_R].update_interval = grid_current_phase_r_sensor_update_interval; grid_current_phase_r_sensor_update_interval_ = grid_current_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_power_phase_r_sensor_update_interval(int grid_power_phase_r_sensor_update_interval) { registers_G3[GRID_POWER_PHASE_R].update_interval = grid_power_phase_r_sensor_update_interval; grid_power_phase_r_sensor_update_interval_ = grid_power_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_voltage_phase_s_sensor_update_interval(int grid_voltage_phase_s_sensor_update_interval) { registers_G3[GRID_VOLTAGE_PHASE_S].update_interval = grid_voltage_phase_s_sensor_update_interval; grid_voltage_phase_s_sensor_update_interval_ = grid_voltage_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_current_phase_s_sensor_update_interval(int grid_current_phase_s_sensor_update_interval) { registers_G3[GRID_CURRENT_PHASE_S].update_interval = grid_current_phase_s_sensor_update_interval; grid_current_phase_s_sensor_update_interval_ = grid_current_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_power_phase_s_sensor_update_interval(int grid_power_phase_s_sensor_update_interval) { registers_G3[GRID_POWER_PHASE_S].update_interval = grid_power_phase_s_sensor_update_interval; grid_power_phase_s_sensor_update_interval_ = grid_power_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_voltage_phase_t_sensor_update_interval(int grid_voltage_phase_t_sensor_update_interval) { registers_G3[GRID_VOLTAGE_PHASE_T].update_interval = grid_voltage_phase_t_sensor_update_interval; grid_voltage_phase_t_sensor_update_interval_ = grid_voltage_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_current_phase_t_sensor_update_interval(int grid_current_phase_t_sensor_update_interval) { registers_G3[GRID_CURRENT_PHASE_T].update_interval = grid_current_phase_t_sensor_update_interval; grid_current_phase_t_sensor_update_interval_ = grid_current_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_power_phase_t_sensor_update_interval(int grid_power_phase_t_sensor_update_interval) { registers_G3[GRID_POWER_PHASE_T].update_interval = grid_power_phase_t_sensor_update_interval; grid_power_phase_t_sensor_update_interval_ = grid_power_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_total_sensor_update_interval(int off_grid_power_total_sensor_update_interval) { registers_G3[OFF_GRID_POWER_TOTAL].update_interval = off_grid_power_total_sensor_update_interval; off_grid_power_total_sensor_update_interval_ = off_grid_power_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_frequency_sensor_update_interval(int off_grid_frequency_sensor_update_interval) { registers_G3[OFF_GRID_FREQUENCY].update_interval = off_grid_frequency_sensor_update_interval; off_grid_frequency_sensor_update_interval_ = off_grid_frequency_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_r_sensor_update_interval(int off_grid_voltage_phase_r_sensor_update_interval) { registers_G3[OFF_GRID_VOLTAGE_PHASE_R].update_interval = off_grid_voltage_phase_r_sensor_update_interval; off_grid_voltage_phase_r_sensor_update_interval_ = off_grid_voltage_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_current_phase_r_sensor_update_interval(int off_grid_current_phase_r_sensor_update_interval) { registers_G3[OFF_GRID_CURRENT_PHASE_R].update_interval = off_grid_current_phase_r_sensor_update_interval; off_grid_current_phase_r_sensor_update_interval_ = off_grid_current_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_phase_r_sensor_update_interval(int off_grid_power_phase_r_sensor_update_interval) { registers_G3[OFF_GRID_POWER_PHASE_R].update_interval = off_grid_power_phase_r_sensor_update_interval; off_grid_power_phase_r_sensor_update_interval_ = off_grid_power_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_s_sensor_update_interval(int off_grid_voltage_phase_s_sensor_update_interval) { registers_G3[OFF_GRID_VOLTAGE_PHASE_S].update_interval = off_grid_voltage_phase_s_sensor_update_interval; off_grid_voltage_phase_s_sensor_update_interval_ = off_grid_voltage_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_current_phase_s_sensor_update_interval(int off_grid_current_phase_s_sensor_update_interval) { registers_G3[OFF_GRID_CURRENT_PHASE_S].update_interval = off_grid_current_phase_s_sensor_update_interval; off_grid_current_phase_s_sensor_update_interval_ = off_grid_current_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_phase_s_sensor_update_interval(int off_grid_power_phase_s_sensor_update_interval) { registers_G3[OFF_GRID_POWER_PHASE_S].update_interval = off_grid_power_phase_s_sensor_update_interval; off_grid_power_phase_s_sensor_update_interval_ = off_grid_power_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_t_sensor_update_interval(int off_grid_voltage_phase_t_sensor_update_interval) { registers_G3[OFF_GRID_VOLTAGE_PHASE_T].update_interval = off_grid_voltage_phase_t_sensor_update_interval; off_grid_voltage_phase_t_sensor_update_interval_ = off_grid_voltage_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_current_phase_t_sensor_update_interval(int off_grid_current_phase_t_sensor_update_interval) { registers_G3[OFF_GRID_CURRENT_PHASE_T].update_interval = off_grid_current_phase_t_sensor_update_interval; off_grid_current_phase_t_sensor_update_interval_ = off_grid_current_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_phase_t_sensor_update_interval(int off_grid_power_phase_t_sensor_update_interval) { registers_G3[OFF_GRID_POWER_PHASE_T].update_interval = off_grid_power_phase_t_sensor_update_interval; off_grid_power_phase_t_sensor_update_interval_ = off_grid_power_phase_t_sensor_update_interval; }


        void SofarSolar_Inverter::set_desired_grid_power_sensor_default_value(int64_t default_value) { registers_G3[DESIRED_GRID_POWER].default_value.int64_value = default_value; registers_G3[18].is_default_value_set = true; }
        void SofarSolar_Inverter::set_minimum_battery_power_sensor_default_value(int64_t default_value) { registers_G3[MINIMUM_BATTERY_POWER].default_value.int64_value = default_value; registers_G3[19].is_default_value_set = true; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor_default_value(int64_t default_value) { registers_G3[MAXIMUM_BATTERY_POWER].default_value.int64_value = default_value; registers_G3[20].is_default_value_set = true; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor_default_value(int64_t default_value) { registers_G3[ENERGY_STORAGE_MODE].default_value.int64_value = default_value; registers_G3[21].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_ID].default_value.int64_value = default_value; registers_G3[22].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_ADDRESS].default_value.int64_value = default_value; registers_G3[23].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_PROTOCOL].default_value.int64_value = default_value; registers_G3[24].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_default_value(float default_value) { registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].default_value.int64_value = (int64_t) (default_value / registers_G3[25].scale); registers_G3[25].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_default_value(float default_value) { registers_G3[BATTERY_CONF_VOLTAGE_OVER].default_value.int64_value = (int64_t) (default_value / registers_G3[26].scale); registers_G3[26].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_default_value(float default_value) { registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].default_value.int64_value = (int64_t) (default_value / registers_G3[27].scale); registers_G3[27].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_default_value(float default_value) { registers_G3[BATTERY_CONF_VOLTAGE_LACK].default_value.int64_value = (int64_t) (default_value / registers_G3[28].scale); registers_G3[28].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_default_value(float default_value) { registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].default_value.int64_value = (int64_t) (default_value / registers_G3[29].scale); registers_G3[29].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_default_value(float default_value) { registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].default_value.int64_value = (int64_t) (default_value / registers_G3[30].scale); registers_G3[30].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_default_value(float default_value) { registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].default_value.int64_value = (int64_t) (default_value / registers_G3[31].scale); registers_G3[31].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].default_value.int64_value = default_value; registers_G3[32].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_END_OF_DISCHARGE].default_value.int64_value = default_value; registers_G3[33].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_CAPACITY].default_value.int64_value = default_value; registers_G3[34].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_CELL_TYPE].default_value.int64_value = default_value; registers_G3[35].is_default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_default_value(int64_t default_value) { registers_G3[BATTERY_CONF_EPS_BUFFER].default_value.int64_value = default_value; registers_G3[36].is_default_value_set = true; }

		void SofarSolar_Inverter::set_desired_grid_power_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[DESIRED_GRID_POWER].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_minimum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[MINIMUM_BATTERY_POWER].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[MAXIMUM_BATTERY_POWER].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[ENERGY_STORAGE_MODE].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_ID].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_ADDRESS].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_PROTOCOL].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_VOLTAGE_OVER].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_VOLTAGE_CHARGE].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_VOLTAGE_LACK].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_END_OF_DISCHARGE].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_CAPACITY].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_CELL_TYPE].enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[BATTERY_CONF_EPS_BUFFER].enforce_default_value = enforce_default_value; }
    }
}