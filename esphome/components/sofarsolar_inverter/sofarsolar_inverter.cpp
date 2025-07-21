#include "queue"
#include "sofarsolar_inverter.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
    namespace sofarsolar_inverter {
        static const char *TAG = "sofarsolar_inverter.component";

		struct SofarSolar_RegisterDynamic {
			uint32_t last_update; // Last update time in milliseconds
			uint16_t update_interval; // Update interval in milliseconds
			sensor::Sensor *sensor; // Pointer to the sensor associated with the register
            SofarSolar_RegisterValue default_value; // Value of the register
			SofarSolar_RegisterValue write_value; // Value to write to the register
            bool default_value_set; // Flag to indicate if the default value is set
			bool enforce_default_value; // Flag to indicate if the default value should be enforced
			bool write_set_value = false; // Flag to indicate if the write value is set
			bool is_queued = false; // Flag to indicate if the register is queued for reading/writing
			SofarSolar_RegisterDynamic() : sensor(nullptr), update_interval(0), last_update(0), default_value({}), default_value_set(false), enforce_default_value(false) {}
		};

		struct register_read_task {
			uint8_t register_key; // Pointer to the register to read
            bool operator<(const register_read_task &other) const {
                return G3_registers.at(this->register_key).priority > G3_registers.at(other.register_key).priority;
            }
		};

        struct register_write_task {
			uint8_t first_register_key; // Pointer to the register to write
			uint8_t number_of_registers; // Number of registers to write
			std::vector<uint8_t> data; // Data to write to the register
            bool operator<(const register_write_task &other) const {
                return G3_registers.at(this->first_register_key).priority > G3_registers.at(other.first_register_key).priority;
            }
        };

        SofarSolar_Inverter::SofarSolar_Inverter() {
			this->G3_dynamic = {
				{PV_GENERATION_TODAY, SofarSolar_RegisterDynamic{}}, // PV Generation Today
        	    {PV_GENERATION_TOTAL, SofarSolar_RegisterDynamic{}}, // PV Generation Total
    	        {LOAD_CONSUMPTION_TODAY, SofarSolar_RegisterDynamic{}}, // Load Consumption Today
    			{LOAD_CONSUMPTION_TOTAL, SofarSolar_RegisterDynamic{}}, // Load Consumption Total
            	{BATTERY_CHARGE_TODAY, SofarSolar_RegisterDynamic{}}, // Battery Charge Today
        	    {BATTERY_CHARGE_TOTAL, SofarSolar_RegisterDynamic{}}, // Battery Charge Total
    	        {BATTERY_DISCHARGE_TODAY, SofarSolar_RegisterDynamic{}}, // Battery Discharge Today
	            {BATTERY_DISCHARGE_TOTAL, SofarSolar_RegisterDynamic{}}, // Battery Discharge Total
            	{TOTAL_ACTIVE_POWER_INVERTER, SofarSolar_RegisterDynamic{}}, // Total Active Power Inverter
        	    {PV_VOLTAGE_1, SofarSolar_RegisterDynamic{}}, // PV Voltage 1
    	        {PV_CURRENT_1, SofarSolar_RegisterDynamic{}}, // PV Current 1
	            {PV_POWER_1, SofarSolar_RegisterDynamic{}}, // PV Power 1
            	{PV_VOLTAGE_2, SofarSolar_RegisterDynamic{}}, // PV Voltage 2
        	    {PV_CURRENT_2, SofarSolar_RegisterDynamic{}}, // PV Current 2
    	        {PV_POWER_2, SofarSolar_RegisterDynamic{}}, // PV Power 2
	            {PV_POWER_TOTAL, SofarSolar_RegisterDynamic{}}, // PV Power Total
            	{BATTERY_POWER_TOTAL, SofarSolar_RegisterDynamic{}}, // Battery Power Total
        	    {BATTERY_STATE_OF_CHARGE_TOTAL, SofarSolar_RegisterDynamic{}}, // Battery State of Charge Total
    	        {DESIRED_GRID_POWER, SofarSolar_RegisterDynamic{}}, // Desired Grid Power
				{MINIMUM_BATTERY_POWER, SofarSolar_RegisterDynamic{}}, // Minimum Battery Power
				{MAXIMUM_BATTERY_POWER, SofarSolar_RegisterDynamic{}}, // Maximum Battery Power
				{ENERGY_STORAGE_MODE, SofarSolar_RegisterDynamic{}}, // Energy Storage Mode
				{BATTERY_CONF_ID, SofarSolar_RegisterDynamic{}}, // Battery Conf ID
				{BATTERY_CONF_ADDRESS, SofarSolar_RegisterDynamic{}}, // Battery Conf Address
				{BATTERY_CONF_PROTOCOL, SofarSolar_RegisterDynamic{}}, // Battery Conf Protocol
				{BATTERY_CONF_VOLTAGE_NOMINAL, SofarSolar_RegisterDynamic{}}, // Battery Conf Voltage Nominal
				{BATTERY_CONF_VOLTAGE_OVER, SofarSolar_RegisterDynamic{}}, // Battery Conf Voltage Over
				{BATTERY_CONF_VOLTAGE_CHARGE, SofarSolar_RegisterDynamic{}}, // Battery Conf Voltage Charge
				{BATTERY_CONF_VOLTAGE_LACK, SofarSolar_RegisterDynamic{}}, // Battery Conf Voltage Lack
				{BATTERY_CONF_VOLTAGE_DISCHARGE_STOP, SofarSolar_RegisterDynamic{}}, // Battery Conf Voltage Discharge Stop
				{BATTERY_CONF_CURRENT_CHARGE_LIMIT, SofarSolar_RegisterDynamic{}}, // Battery Conf Current Charge Limit
				{BATTERY_CONF_CURRENT_DISCHARGE_LIMIT, SofarSolar_RegisterDynamic{}}, // Battery Conf Current Discharge Limit
				{BATTERY_CONF_DEPTH_OF_DISCHARGE, SofarSolar_RegisterDynamic{}}, // Battery Conf Depth of Discharge
				{BATTERY_CONF_END_OF_DISCHARGE, SofarSolar_RegisterDynamic{}}, // Battery Conf End of Discharge
				{BATTERY_CONF_CAPACITY, SofarSolar_RegisterDynamic{}}, // Battery Conf Capacity
				{BATTERY_CONF_CELL_TYPE, SofarSolar_RegisterDynamic{}}, // Battery Conf Cell Type
				{BATTERY_CONF_EPS_BUFFER, SofarSolar_RegisterDynamic{}}, // Battery Conf EPS Buffer
				{BATTERY_CONF_CONTROL, SofarSolar_RegisterDynamic{}}, // Battery Conf Control
				{GRID_FREQUENCY, SofarSolar_RegisterDynamic{}}, // Grid Frequency
				{GRID_VOLTAGE_PHASE_R, SofarSolar_RegisterDynamic{}}, // Grid Voltage Phase R
				{GRID_CURRENT_PHASE_R, SofarSolar_RegisterDynamic{}}, // Grid Current Phase R
				{GRID_POWER_PHASE_R, SofarSolar_RegisterDynamic{}}, // Grid Power Phase R
				{GRID_VOLTAGE_PHASE_S, SofarSolar_RegisterDynamic{}}, // Grid Voltage Phase S
				{GRID_CURRENT_PHASE_S, SofarSolar_RegisterDynamic{}}, // Grid Current Phase S
				{GRID_POWER_PHASE_S, SofarSolar_RegisterDynamic{}}, // Grid Power Phase S
				{GRID_VOLTAGE_PHASE_T, SofarSolar_RegisterDynamic{}}, // Grid Voltage Phase T
				{GRID_CURRENT_PHASE_T, SofarSolar_RegisterDynamic{}}, // Grid Current Phase T
				{GRID_POWER_PHASE_T, SofarSolar_RegisterDynamic{}}, // Grid Power Phase T
				{OFF_GRID_POWER_TOTAL, SofarSolar_RegisterDynamic{}}, // Off Grid Power Total
				{OFF_GRID_FREQUENCY, SofarSolar_RegisterDynamic{}}, // Off Grid Frequency
				{OFF_GRID_VOLTAGE_PHASE_R, SofarSolar_RegisterDynamic{}}, // Off Grid Voltage Phase R
				{OFF_GRID_CURRENT_PHASE_R, SofarSolar_RegisterDynamic{}}, // Off Grid Current Phase R
				{OFF_GRID_POWER_PHASE_R, SofarSolar_RegisterDynamic{}}, // Off Grid Power Phase R
				{OFF_GRID_VOLTAGE_PHASE_S, SofarSolar_RegisterDynamic{}}, // Off Grid Voltage Phase S
				{OFF_GRID_CURRENT_PHASE_S, SofarSolar_RegisterDynamic{}}, // Off Grid Current Phase S
				{OFF_GRID_POWER_PHASE_S, SofarSolar_RegisterDynamic{}}, // Off Grid Power Phase S
				{OFF_GRID_VOLTAGE_PHASE_T, SofarSolar_RegisterDynamic{}}, // Off Grid Voltage Phase T
				{OFF_GRID_CURRENT_PHASE_T, SofarSolar_RegisterDynamic{}}, // Off Grid Current Phase T
				{OFF_GRID_POWER_PHASE_T, SofarSolar_RegisterDynamic{}}, // Off Grid Power Phase T
				{BATTERY_ACTIVE_CONTROL, SofarSolar_RegisterDynamic{}}, // Battery Active Control
				{BATTERY_ACTIVE_ONESHOT, SofarSolar_RegisterDynamic{}} // Battery Active Oneshot
        	};
        }

        int time_last_loop = 0;
        bool current_reading = false; // Pointer to the current reading task
        bool current_writing = false; // Pointer to the current writing task
        register_write_task current_write_task; // Pointer to the current writing task
        uint32_t time_begin_modbus_operation = 0;
        uint32_t zero_export_last_update = 0;
        std::priority_queue<register_read_task> register_read_queue; // Priority queue for register read tasks
        std::priority_queue<register_write_task> register_write_queue; // Priority queue for register write tasks

        void SofarSolar_Inverter::setup() {
			ESP_LOGCONFIG(TAG, "Setting up Sofar Solar Inverter");
            G3_dynamic.at(BATTERY_ACTIVE_CONTROL).write_value.uint16_value = 1;
            G3_dynamic.at(BATTERY_ACTIVE_CONTROL).write_set_value = true;
            G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).write_value.uint16_value = 1;
            G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).write_set_value = true;
            time_begin_modbus_operation = millis();
            this->write_battery_active(); // Write the battery active control register
        }

        void SofarSolar_Inverter::loop() {
			if (millis() - zero_export_last_update > 1000 && this->zero_export_) {
				zero_export_last_update = millis();
				ESP_LOGD(TAG, "Updating zero export status");
				// Read the current zero export status
				G3_dynamic.at(DESIRED_GRID_POWER).write_value.int32_value = G3_dynamic.at(TOTAL_ACTIVE_POWER_INVERTER).sensor->state + this->power_sensor_->state;
				G3_dynamic.at(DESIRED_GRID_POWER).write_set_value = true;
				G3_dynamic.at(MINIMUM_BATTERY_POWER).write_value.int32_value = -5000;
				G3_dynamic.at(MINIMUM_BATTERY_POWER).write_set_value = true;
				G3_dynamic.at(MAXIMUM_BATTERY_POWER).write_value.int32_value = 5000;
				G3_dynamic.at(MAXIMUM_BATTERY_POWER).write_set_value = true;
				ESP_LOGD(TAG, "Current total active power inverter: %f W, Current power sensor: %f W, New desired grid power: %d W", G3_dynamic.at(TOTAL_ACTIVE_POWER_INVERTER).sensor->state, this->power_sensor_->state, G3_dynamic.at(DESIRED_GRID_POWER).write_value.int32_value);
				this->write_desired_grid_power(); // Write the new desired grid power, minimum battery power, and maximum battery power
			}

            for (auto &dynamic_register : this->G3_dynamic) {
				ESP_LOGVV(TAG, "Checking register %d for update. Last update %d, Update Intervall %d", dynamic_register.first, millis() - dynamic_register.second.last_update, dynamic_register.second.update_interval);
                if (dynamic_register.second.sensor == nullptr) {
					continue; // Skip if the sensor pointer is null
				}
		        if (millis() - dynamic_register.second.last_update >= dynamic_register.second.update_interval  && !dynamic_register.second.is_queued) {
					dynamic_register.second.last_update = millis(); // Update the last update time
                    register_read_task task;
					task.register_key = dynamic_register.first; // Set the register key for the task
                    dynamic_register.second.is_queued = true; // Mark the register as queued
                    register_read_queue.push(task); // Add the task to the read queue
                    ESP_LOGVV(TAG, "Queued register %s for reading", dynamic_register.first.c_str());
                }
            }

			if(!current_reading && !current_writing && !register_write_queue.empty() && millis() - time_begin_modbus_operation > 150) {
				// If there is a write task in the queue, process it
				write_modbus_register(G3_registers.at(register_write_queue.top().first_register_key).start_address, register_write_queue.top().number_of_registers, register_write_queue.top().data); // Write the register
				current_writing = true; // Set the flag to indicate that a write is in progress
				time_begin_modbus_operation = millis(); // Record the start time of the Modbus operation
			}

			if (!current_reading && !current_writing && !register_read_queue.empty() && millis() - time_begin_modbus_operation > 150) {
				read_modbus_register(G3_registers.at(register_read_queue.top().register_key).start_address, G3_registers.at(register_read_queue.top().register_key).register_count);
				current_reading = true; // Set the flag to indicate that a read is in progress
				time_begin_modbus_operation = millis(); // Record the start time of the Modbus operation
			}

			if (millis() - time_begin_modbus_operation > 500) { // Timeout for read operation
				if (current_reading) {
					current_reading = false; // Reset the flag for read operation
					register_read_queue.pop(); // Remove the top task from the read queue
					ESP_LOGE(TAG, "Modbus read operation timed out");
				} else if (current_writing) {
					current_writing = false; // Reset the flag for write operation
					register_write_queue.pop(); // Remove the top task from the write queue
					ESP_LOGE(TAG, "Modbus write operation timed out");
				}
			}
        }

		void SofarSolar_Inverter::on_modbus_data(const std::vector<uint8_t> &data) {
            ESP_LOGD(TAG, "Received Modbus data: %s", vector_to_string(data).c_str());
            if(current_reading) {
				parse_read_response(data);
				time_begin_modbus_operation = millis(); // Reset the start time of the Modbus operation
				G3_dynamic.at(register_read_queue.top().register_key).is_queued = false; // Mark the register as not queued
				current_reading = false; // Reset the flag for read operation
				register_read_queue.pop(); // Remove the top task from the read queue
			} else if (current_writing) {
				parse_write_response(data);
				time_begin_modbus_operation = millis(); // Reset the start time of the Modbus operation
				current_writing = false; // Reset the flag for read operation
				register_write_queue.pop(); // Remove the top task from the read queue
			} else {
				ESP_LOGE(TAG, "Received Modbus data while not in a read or write operation");
			}
        }

        void SofarSolar_Inverter::on_modbus_error(uint8_t function_code, uint8_t exception_code) {
            ESP_LOGE(TAG, "Modbus error: Function code %02X, Exception code %02X", function_code, exception_code);
			if (function_code == 0x90) {
                switch (exception_code) {
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
                        ESP_LOGE(TAG, "Modbus error: Unknown error code %02X", exception_code);
                }
            }
        }

		void SofarSolar_Inverter::parse_read_response(const std::vector<uint8_t> &data) {
            ESP_LOGVV(TAG, "Parsing read response: %s", vector_to_string(data).c_str());
            switch (G3_registers.at(register_read_queue.top().register_key).type) {
                case U_WORD: {
                    if (data.size() != 2) {
                        ESP_LOGE(TAG, "Invalid read response size for U_WORD: %d", data.size());
                        return;
                    }
                    uint16_t value = (data[0] << 8) | data[1];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(static_cast<float>(value) * get_power_of_ten(G3_registers.at(register_read_queue.top().register_key).scale));
                    break;
				}
				case S_WORD: {
                    if (data.size() != 2) {
                        ESP_LOGE(TAG, "Invalid read response size for S_WORD: %d", data.size());
                        return;
                    }
                    int16_t value = (data[0] << 8) | data[1];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(static_cast<float>(value) * get_power_of_ten(G3_registers.at(register_read_queue.top().register_key).scale));
                    break;
				}
				case U_DWORD: {
                    if (data.size() != 4) {
                        ESP_LOGE(TAG, "Invalid read response size for U_DWORD: %d", data.size());
                        return;
                    }
                    uint32_t value = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(static_cast<float>(value) * get_power_of_ten(G3_registers.at(register_read_queue.top().register_key).scale));
                    break;
				}
				case S_DWORD: {
                    if (data.size() != 4) {
                        ESP_LOGE(TAG, "Invalid read response size for S_DWORD: %d", data.size());
                        return;
                    }
                    int32_t value = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(static_cast<float>(value) * get_power_of_ten(G3_registers.at(register_read_queue.top().register_key).scale));
                    break;
				}
				default:
                    ESP_LOGE(TAG, "Unsupported register type for read response: %d", G3_registers.at(register_read_queue.top().register_key).type);
                    return;
            }
        }

		void SofarSolar_Inverter::parse_write_response(const std::vector<uint8_t> &data) {
        	ESP_LOGVV(TAG, "Parsing write response: %s", vector_to_string(data).c_str());
			if (data.size() != 4) {
				ESP_LOGE(TAG, "Invalid write response size: %d", data.size());
			}
			if (G3_registers.at(register_write_queue.top().first_register_key).start_address != ((data[0] << 8) | data[1])) {
				ESP_LOGE(TAG, "Invalid response address: expected %04X, got %02X%02X", G3_registers.at(register_write_queue.top().first_register_key).start_address, data[2], data[3]);
				return; // Invalid response address
			}
			if (register_write_queue.top().number_of_registers != ((data[2] << 8) | data[3])) {
				ESP_LOGE(TAG, "Invalid response quantity: expected %d, got %02X", register_write_queue.top().number_of_registers, ((data[2] << 8) | data[3]));
				return; // Invalid response quantity
			}
		};

            //if (response.data()[2] != response.size() - 5 && response.data()[2] != register_info.quantity * 2) {
            //    ESP_LOGE(TAG, "Invalid response size: expected %d, got %d", response.data()[2], response.size() - 5);
            //    response.clear(); // Clear the response on invalid size
            //    return false; // Invalid response size
            //}


            //if (response.data()[1] != 0x10) {
            //    ESP_LOGE(TAG, "Invalid Modbus response function code: %02X", response.data()[1]);
            //    response.clear(); // Clear the response on invalid function code
            //    return false; // Invalid function code
            //}
            //if (task.register_ptr->start_address != ((response.data()[2] << 8) | (response.data()[3]))) {
            //    ESP_LOGE(TAG, "Invalid response address: expected %04X, got %02X%02X", task.register_ptr->start_address, response.data()[2], response.data()[3]);
            //    return false; // Invalid response size
            //}
            //if (task.number_of_registers != ((response.data()[4] << 8) | (response.data()[5]))) {
            //    ESP_LOGE(TAG, "Invalid response quantity: expected %d, got %02X", task.number_of_registers, ((response.data()[4] << 8) | (response.data()[5])));
            //    return false; // Invalid response size
            //}


        void SofarSolar_Inverter::dump_config(){
            ESP_LOGCONFIG(TAG, "SofarSolar_Inverter");
            ESP_LOGCONFIG(TAG, "  model = %s", this->model_.c_str());
            ESP_LOGCONFIG(TAG, "  modbus_address = %i", this->modbus_address_);
            ESP_LOGCONFIG(TAG, "  zero_export = %s", TRUEFALSE(this->zero_export_));
            ESP_LOGCONFIG(TAG, "  power_sensor = %s", this->power_sensor_ ? this->power_sensor_->get_name().c_str() : "None");
            for (const auto &reg : G3_registers) {
                ESP_LOGCONFIG(TAG, "  %s: start_address = %04X, type = %d, scale = %f, enforce_default_value = %s",
                              G3_dynamic.at(reg.first).sensor->get_name().c_str(), reg.second.start_address, reg.second.type, reg.second.scale,
                              TRUEFALSE(G3_dynamic.at(enforce_default_value)));
            }
        }

        void SofarSolar_Inverter::read_modbus_register(uint16_t start_address, uint16_t register_count) {
            // Create Modbus frame for reading registers
			std::vector<uint8_t> frame = {static_cast<uint8_t>(this->modbus_address_), 0x03, static_cast<uint8_t>(start_address >> 8), static_cast<uint8_t>(start_address & 0xFF), static_cast<uint8_t>(register_count >> 8), static_cast<uint8_t>(register_count & 0xFF)};
			ESP_LOGD(TAG, "Reading Modbus registers: %s", vector_to_string(frame).c_str());
            this->send_raw(frame);
        }

        void SofarSolar_Inverter::write_modbus_register(uint16_t start_address, uint16_t register_count, const std::vector<uint8_t> &data) {
            // Create Modbus frame for writing registers
            std::vector<uint8_t> frame = {static_cast<uint8_t>(this->modbus_address_), 0x10, static_cast<uint8_t>(start_address >> 8), static_cast<uint8_t>(start_address & 0xFF), static_cast<uint8_t>(register_count >> 8), static_cast<uint8_t>(register_count & 0xFF), static_cast<uint8_t>(data.size())};
            frame.insert(frame.end(), data.begin(), data.end());
			ESP_LOGD(TAG, "Writing Modbus registers: %s", vector_to_string(frame).c_str());
            this->send_raw(frame);
        }

        void SofarSolar_Inverter::write_desired_grid_power() {
            // Write the desired grid power, minimum battery power, and maximum battery power
            int32_t new_desired_grid_power;
            if (G3_dynamic.at(DESIRED_GRID_POWER).enforce_default_value && G3_dynamic.at(DESIRED_GRID_POWER).default_value_set) {
                new_desired_grid_power = G3_dynamic.at(DESIRED_GRID_POWER).default_value.int32_value;
            } else if (G3_dynamic.at(DESIRED_GRID_POWER).write_set_value) {
                new_desired_grid_power = G3_dynamic.at(DESIRED_GRID_POWER).write_value.int32_value;
            } else {
                new_desired_grid_power = G3_dynamic.at(DESIRED_GRID_POWER).sensor->state;
            }
            int32_t new_minimum_battery_power;
            if (G3_dynamic.at(MINIMUM_BATTERY_POWER).enforce_default_value && G3_dynamic.at(MINIMUM_BATTERY_POWER).default_value_set) {
                new_minimum_battery_power = G3_dynamic.at(MINIMUM_BATTERY_POWER).default_value.int32_value;
            } else if (G3_dynamic.at(MINIMUM_BATTERY_POWER).write_set_value) {
                new_minimum_battery_power = G3_dynamic.at(MINIMUM_BATTERY_POWER).write_value.int32_value;
            } else {
                new_minimum_battery_power = G3_dynamic.at(MINIMUM_BATTERY_POWER).sensor->state;
            }
            int32_t new_maximum_battery_power;
            if (G3_dynamic.at(MAXIMUM_BATTERY_POWER).enforce_default_value && G3_dynamic.at(MAXIMUM_BATTERY_POWER).default_value_set) {
                new_maximum_battery_power = G3_dynamic.at(MAXIMUM_BATTERY_POWER).default_value.int32_value;
            } else if (G3_dynamic.at(MAXIMUM_BATTERY_POWER).write_set_value) {
                new_maximum_battery_power = G3_dynamic.at(MAXIMUM_BATTERY_POWER).write_value.int32_value;
            } else {
                new_maximum_battery_power = G3_dynamic.at(MAXIMUM_BATTERY_POWER).sensor->state;
            }
            ESP_LOGD(TAG, "Writing desired grid power: %d W", new_desired_grid_power);
            ESP_LOGD(TAG, "Writing minimum battery power: %d W", new_minimum_battery_power);
            ESP_LOGD(TAG, "Writing maximum battery power: %d W", new_maximum_battery_power);
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
        	register_write_task task;
        	task.first_register_key = DESIRED_GRID_POWER; // Set the register key for the write task
        	task.number_of_registers = (data.size() >> 1); // Set the number of registers to write
			ESP_LOGD(TAG, "Number of registers to write: %d", task.number_of_registers);
        	task.data = data; // Set the data to write
        	register_write_queue.push(task); // Add the write task to the queue
        }

        void SofarSolar_Inverter::write_battery_conf() {
            // Write the battery configuration
            std::vector<uint8_t> data;
            ESP_LOGD(TAG, "Writing battery configuration");
            uint16_t new_battery_conf_id;
            if (G3_dynamic.at(BATTERY_CONF_ID).enforce_default_value && G3_dynamic.at(BATTERY_CONF_ID).default_value_set) {
                new_battery_conf_id = G3_dynamic.at(BATTERY_CONF_ID).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_ID).write_set_value) {
                new_battery_conf_id = G3_dynamic.at(BATTERY_CONF_ID).write_value.uint16_value;
            } else {
                new_battery_conf_id = G3_dynamic.at(BATTERY_CONF_ID).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_id >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_id & 0xFF));
            uint16_t new_battery_conf_address;
            if (G3_dynamic.at(BATTERY_CONF_ADDRESS).enforce_default_value && G3_dynamic.at(BATTERY_CONF_ADDRESS).default_value_set) {
                new_battery_conf_address = G3_dynamic.at(BATTERY_CONF_ADDRESS).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_ADDRESS).write_set_value) {
                new_battery_conf_address = G3_dynamic.at(BATTERY_CONF_ADDRESS).write_value.uint16_value;
            } else {
                new_battery_conf_address = G3_dynamic.at(BATTERY_CONF_ADDRESS).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_address >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_address & 0xFF));
            uint16_t new_battery_conf_protocol;
            if (G3_dynamic.at(BATTERY_CONF_PROTOCOL).enforce_default_value && G3_dynamic.at(BATTERY_CONF_PROTOCOL).default_value_set) {
                new_battery_conf_protocol = G3_dynamic.at(BATTERY_CONF_PROTOCOL).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_PROTOCOL).write_set_value) {
                new_battery_conf_protocol = G3_dynamic.at(BATTERY_CONF_PROTOCOL).write_value.uint16_value;
            } else {
                new_battery_conf_protocol = G3_dynamic.at(BATTERY_CONF_PROTOCOL).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_protocol >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_protocol & 0xFF));
            uint16_t new_battery_conf_voltage_over;
            if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).enforce_default_value && G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).default_value_set) {
                new_battery_conf_voltage_over = G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).write_set_value) {
                new_battery_conf_voltage_over = G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).write_value.uint16_value;
            } else {
                new_battery_conf_voltage_over = G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_over >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_over & 0xFF));
            uint16_t new_battery_conf_voltage_charge;
            if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).enforce_default_value && G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).default_value_set) {
                new_battery_conf_voltage_charge = G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).write_set_value) {
                new_battery_conf_voltage_charge = G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).write_value.uint16_value;
            } else {
                new_battery_conf_voltage_charge = G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_charge >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_charge & 0xFF));
            uint16_t new_battery_conf_voltage_lack;
            if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).enforce_default_value && G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).default_value_set) {
                new_battery_conf_voltage_lack = G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).write_set_value) {
                new_battery_conf_voltage_lack = G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).write_value.uint16_value;
            } else {
                new_battery_conf_voltage_lack = G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_lack >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_lack & 0xFF));
            uint16_t new_battery_conf_voltage_discharge_stop;
            if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).enforce_default_value && G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).default_value_set) {
                new_battery_conf_voltage_discharge_stop = G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).write_set_value) {
                new_battery_conf_voltage_discharge_stop = G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).write_value.uint16_value;
            } else {
                new_battery_conf_voltage_discharge_stop = G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_discharge_stop >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_discharge_stop & 0xFF));
            uint16_t new_battery_conf_current_charge_limit;
            if (G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).enforce_default_value && G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).default_value_set) {
                new_battery_conf_current_charge_limit = G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).write_set_value) {
                new_battery_conf_current_charge_limit = G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).write_value.uint16_value;
            } else {
                new_battery_conf_current_charge_limit = G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_charge_limit >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_charge_limit & 0xFF));
            uint16_t new_battery_conf_current_discharge_limit;
            if (G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).enforce_default_value && G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).default_value_set) {
                new_battery_conf_current_discharge_limit = G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).write_set_value) {
                new_battery_conf_current_discharge_limit = G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).write_value.uint16_value;
            } else {
                new_battery_conf_current_discharge_limit = G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_discharge_limit >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_current_discharge_limit & 0xFF));
            uint16_t new_battery_conf_depth_of_discharge;
            if (G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).enforce_default_value && G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).default_value_set) {
                new_battery_conf_depth_of_discharge = G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).write_set_value) {
                new_battery_conf_depth_of_discharge = G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).write_value.uint16_value;
            } else {
                new_battery_conf_depth_of_discharge = G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_depth_of_discharge >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_depth_of_discharge & 0xFF));
            uint16_t new_battery_conf_end_of_discharge;
            if (G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).enforce_default_value && G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).default_value_set) {
                new_battery_conf_end_of_discharge = G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).write_set_value) {
                new_battery_conf_end_of_discharge = G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).write_value.uint16_value;
            } else {
                new_battery_conf_end_of_discharge = G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_end_of_discharge >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_end_of_discharge & 0xFF));
            uint16_t new_battery_conf_capacity;
            if (G3_dynamic.at(BATTERY_CONF_CAPACITY).enforce_default_value && G3_dynamic.at(BATTERY_CONF_CAPACITY).default_value_set) {
                new_battery_conf_capacity = G3_dynamic.at(BATTERY_CONF_CAPACITY).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_CAPACITY).write_set_value) {
                new_battery_conf_capacity = G3_dynamic.at(BATTERY_CONF_CAPACITY).write_value.uint16_value;
            } else {
                new_battery_conf_capacity = G3_dynamic.at(BATTERY_CONF_CAPACITY).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_capacity >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_capacity & 0xFF));
            uint16_t new_battery_conf_voltage_nominal;
            if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).enforce_default_value && G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).default_value_set) {
                new_battery_conf_voltage_nominal = G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).write_set_value) {
                new_battery_conf_voltage_nominal = G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).write_value.uint16_value;
            } else {
                new_battery_conf_voltage_nominal = G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_nominal >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_voltage_nominal & 0xFF));
            uint16_t new_battery_conf_cell_type;
            if (G3_dynamic.at(BATTERY_CONF_CELL_TYPE).enforce_default_value && G3_dynamic.at(BATTERY_CONF_CELL_TYPE).default_value_set) {
                new_battery_conf_cell_type = G3_dynamic.at(BATTERY_CONF_CELL_TYPE).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_CELL_TYPE).write_set_value) {
                new_battery_conf_cell_type = G3_dynamic.at(BATTERY_CONF_CELL_TYPE).write_value.uint16_value;
            } else {
                new_battery_conf_cell_type = G3_dynamic.at(BATTERY_CONF_CELL_TYPE).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_cell_type >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_cell_type & 0xFF));
            uint16_t new_battery_conf_eps_buffer;
            if (G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).enforce_default_value && G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).default_value_set) {
                new_battery_conf_eps_buffer = G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).write_set_value) {
                new_battery_conf_eps_buffer = G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).write_value.uint16_value;
            } else {
                new_battery_conf_eps_buffer = G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_conf_eps_buffer >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_conf_eps_buffer & 0xFF));
            data.push_back(static_cast<uint8_t>(0x01 >> 8));
            data.push_back(static_cast<uint8_t>(0x01 & 0xFF)); // Write the battery configuration
        	register_write_task task;
        	task.first_register_key = BATTERY_CONF_ID; // Set the register key for the write task
        	task.number_of_registers = (data.size() >> 1); // Set the number of registers to write
        	task.data = data; // Set the data to write
        	register_write_queue.push(task); // Add the write task to the queue
        }

		void SofarSolar_Inverter::write_battery_active() {
            // Write the battery active state
            ESP_LOGD(TAG, "Writing battery active state");
            std::vector<uint8_t> data;
            uint16_t new_battery_active_control;
            if (G3_dynamic.at(BATTERY_ACTIVE_CONTROL).enforce_default_value && G3_dynamic.at(BATTERY_ACTIVE_CONTROL).default_value_set) {
                new_battery_active_control = G3_dynamic.at(BATTERY_ACTIVE_CONTROL).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_ACTIVE_CONTROL).write_set_value) {
                new_battery_active_control = G3_dynamic.at(BATTERY_ACTIVE_CONTROL).write_value.uint16_value;
            } else {
                new_battery_active_control = G3_dynamic.at(BATTERY_ACTIVE_CONTROL).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_active_control >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_active_control & 0xFF));
            uint16_t new_battery_active_oneshot;
            if (G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).enforce_default_value && G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).default_value_set) {
                new_battery_active_oneshot = G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).default_value.uint16_value;
            } else if (G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).write_set_value) {
                new_battery_active_oneshot = G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).write_value.uint16_value;
            } else {
                new_battery_active_oneshot = G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_active_oneshot >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_active_oneshot & 0xFF));
        	register_write_task task;
        	task.first_register_key = BATTERY_ACTIVE_CONTROL; // Set the register key for the write task
			task.number_of_registers = (data.size() >> 1); // Set the number of registers to write
			task.data = data; // Set the data to write
        	register_write_queue.push(task); // Add the write task to the queue
        }

        void SofarSolar_Inverter::write_single_register() {

        }

        void SofarSolar_Inverter::set_pv_generation_today_sensor(sensor::Sensor *pv_generation_today_sensor) { G3_dynamic.at(PV_GENERATION_TODAY).sensor = pv_generation_today_sensor; }
		void SofarSolar_Inverter::set_pv_generation_total_sensor(sensor::Sensor *pv_generation_total_sensor) { G3_dynamic.at(PV_GENERATION_TOTAL).sensor = pv_generation_total_sensor; }
		void SofarSolar_Inverter::set_load_consumption_today_sensor(sensor::Sensor *load_consumption_today_sensor) { G3_dynamic.at(LOAD_CONSUMPTION_TODAY).sensor = load_consumption_today_sensor; }
		void SofarSolar_Inverter::set_load_consumption_total_sensor(sensor::Sensor *load_consumption_total_sensor) { G3_dynamic.at(LOAD_CONSUMPTION_TOTAL).sensor = load_consumption_total_sensor; }
		void SofarSolar_Inverter::set_battery_charge_today_sensor(sensor::Sensor *battery_charge_today_sensor) { G3_dynamic.at(BATTERY_CHARGE_TODAY).sensor = battery_charge_today_sensor; }
		void SofarSolar_Inverter::set_battery_charge_total_sensor(sensor::Sensor *battery_charge_total_sensor) { G3_dynamic.at(BATTERY_CHARGE_TOTAL).sensor = battery_charge_total_sensor; }
		void SofarSolar_Inverter::set_battery_discharge_today_sensor(sensor::Sensor *battery_discharge_today_sensor) { G3_dynamic.at(BATTERY_DISCHARGE_TODAY).sensor = battery_discharge_today_sensor; }
		void SofarSolar_Inverter::set_battery_discharge_total_sensor(sensor::Sensor *battery_discharge_total_sensor) { G3_dynamic.at(BATTERY_DISCHARGE_TOTAL).sensor = battery_discharge_total_sensor; }
		void SofarSolar_Inverter::set_total_active_power_inverter_sensor(sensor::Sensor *total_active_power_inverter_sensor) { G3_dynamic.at(TOTAL_ACTIVE_POWER_INVERTER).sensor = total_active_power_inverter_sensor; }
		void SofarSolar_Inverter::set_pv_voltage_1_sensor(sensor::Sensor *pv_voltage_1_sensor) { G3_dynamic.at(PV_VOLTAGE_1).sensor = pv_voltage_1_sensor; }
		void SofarSolar_Inverter::set_pv_current_1_sensor(sensor::Sensor *pv_current_1_sensor) { G3_dynamic.at(PV_CURRENT_1).sensor = pv_current_1_sensor; }
		void SofarSolar_Inverter::set_pv_power_1_sensor(sensor::Sensor *pv_power_1_sensor) { G3_dynamic.at(PV_POWER_1).sensor = pv_power_1_sensor; }
		void SofarSolar_Inverter::set_pv_voltage_2_sensor(sensor::Sensor *pv_voltage_2_sensor) { G3_dynamic.at(PV_VOLTAGE_2).sensor = pv_voltage_2_sensor; }
		void SofarSolar_Inverter::set_pv_current_2_sensor(sensor::Sensor *pv_current_2_sensor) { G3_dynamic.at(PV_CURRENT_2).sensor = pv_current_2_sensor; }
		void SofarSolar_Inverter::set_pv_power_2_sensor(sensor::Sensor *pv_power_2_sensor) { G3_dynamic.at(PV_POWER_2).sensor = pv_power_2_sensor; }
		void SofarSolar_Inverter::set_pv_power_total_sensor(sensor::Sensor *pv_power_total_sensor) { G3_dynamic.at(PV_POWER_TOTAL).sensor = pv_power_total_sensor; }
		void SofarSolar_Inverter::set_battery_power_total_sensor(sensor::Sensor *battery_power_total_sensor) { G3_dynamic.at(BATTERY_POWER_TOTAL).sensor = battery_power_total_sensor; }
		void SofarSolar_Inverter::set_battery_state_of_charge_total_sensor(sensor::Sensor *battery_state_of_charge_total_sensor) { G3_dynamic.at(BATTERY_STATE_OF_CHARGE_TOTAL).sensor = battery_state_of_charge_total_sensor; }
		void SofarSolar_Inverter::set_desired_grid_power_sensor(sensor::Sensor *desired_grid_power_sensor) { G3_dynamic.at(DESIRED_GRID_POWER).sensor = desired_grid_power_sensor; }
		void SofarSolar_Inverter::set_minimum_battery_power_sensor(sensor::Sensor *minimum_battery_power_sensor) { G3_dynamic.at(MINIMUM_BATTERY_POWER).sensor = minimum_battery_power_sensor; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor(sensor::Sensor *maximum_battery_power_sensor) { G3_dynamic.at(MAXIMUM_BATTERY_POWER).sensor = maximum_battery_power_sensor; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor(sensor::Sensor *energy_storage_mode_sensor) { G3_dynamic.at(ENERGY_STORAGE_MODE).sensor = energy_storage_mode_sensor; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor(sensor::Sensor *battery_conf_id_sensor) { G3_dynamic.at(BATTERY_CONF_ID).sensor = battery_conf_id_sensor; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor(sensor::Sensor *battery_conf_address_sensor) { G3_dynamic.at(BATTERY_CONF_ADDRESS).sensor = battery_conf_address_sensor; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor(sensor::Sensor *battery_conf_protocol_sensor) { G3_dynamic.at(BATTERY_CONF_PROTOCOL).sensor = battery_conf_protocol_sensor; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor(sensor::Sensor *battery_conf_voltage_nominal_sensor) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).sensor = battery_conf_voltage_nominal_sensor; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor(sensor::Sensor *battery_conf_voltage_over_sensor) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).sensor = battery_conf_voltage_over_sensor; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor(sensor::Sensor *battery_conf_voltage_charge_sensor) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).sensor = battery_conf_voltage_charge_sensor; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor(sensor::Sensor *battery_conf_voltage_lack_sensor) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).sensor = battery_conf_voltage_lack_sensor; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor(sensor::Sensor *battery_conf_voltage_discharge_stop_sensor) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).sensor = battery_conf_voltage_discharge_stop_sensor; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor(sensor::Sensor *battery_conf_current_charge_limit_sensor) { G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).sensor = battery_conf_current_charge_limit_sensor; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor(sensor::Sensor *battery_conf_current_discharge_limit_sensor) { G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).sensor = battery_conf_current_discharge_limit_sensor; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor(sensor::Sensor *battery_conf_depth_of_discharge_sensor) { G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).sensor = battery_conf_depth_of_discharge_sensor; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor(sensor::Sensor *battery_conf_end_of_discharge_sensor) { G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).sensor = battery_conf_end_of_discharge_sensor; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor(sensor::Sensor *battery_conf_capacity_sensor) { G3_dynamic.at(BATTERY_CONF_CAPACITY).sensor = battery_conf_capacity_sensor; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor(sensor::Sensor *battery_conf_cell_type_sensor) { G3_dynamic.at(BATTERY_CONF_CELL_TYPE).sensor = battery_conf_cell_type_sensor; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor(sensor::Sensor *battery_conf_eps_buffer_sensor) { G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).sensor = battery_conf_eps_buffer_sensor; }
		void SofarSolar_Inverter::set_battery_conf_control_sensor(sensor::Sensor *battery_conf_control_sensor) { G3_dynamic.at(BATTERY_CONF_CONTROL).sensor = battery_conf_control_sensor; }
		void SofarSolar_Inverter::set_grid_frequency_sensor(sensor::Sensor *grid_frequency_sensor) { G3_dynamic.at(GRID_FREQUENCY).sensor = grid_frequency_sensor; }
		void SofarSolar_Inverter::set_grid_voltage_phase_r_sensor(sensor::Sensor *grid_voltage_phase_r_sensor) { G3_dynamic.at(GRID_VOLTAGE_PHASE_R).sensor = grid_voltage_phase_r_sensor; }
		void SofarSolar_Inverter::set_grid_current_phase_r_sensor(sensor::Sensor *grid_current_phase_r_sensor) { G3_dynamic.at(GRID_CURRENT_PHASE_R).sensor = grid_current_phase_r_sensor; }
		void SofarSolar_Inverter::set_grid_power_phase_r_sensor(sensor::Sensor *grid_power_phase_r_sensor) { G3_dynamic.at(GRID_POWER_PHASE_R).sensor = grid_power_phase_r_sensor; }
		void SofarSolar_Inverter::set_grid_voltage_phase_s_sensor(sensor::Sensor *grid_voltage_phase_s_sensor) { G3_dynamic.at(GRID_VOLTAGE_PHASE_S).sensor = grid_voltage_phase_s_sensor; }
		void SofarSolar_Inverter::set_grid_current_phase_s_sensor(sensor::Sensor *grid_current_phase_s_sensor) { G3_dynamic.at(GRID_CURRENT_PHASE_S).sensor = grid_current_phase_s_sensor; }
		void SofarSolar_Inverter::set_grid_power_phase_s_sensor(sensor::Sensor *grid_power_phase_s_sensor) { G3_dynamic.at(GRID_POWER_PHASE_S).sensor = grid_power_phase_s_sensor; }
		void SofarSolar_Inverter::set_grid_voltage_phase_t_sensor(sensor::Sensor *grid_voltage_phase_t_sensor) { G3_dynamic.at(GRID_VOLTAGE_PHASE_T).sensor = grid_voltage_phase_t_sensor; }
		void SofarSolar_Inverter::set_grid_current_phase_t_sensor(sensor::Sensor *grid_current_phase_t_sensor) { G3_dynamic.at(GRID_CURRENT_PHASE_T).sensor = grid_current_phase_t_sensor; }
		void SofarSolar_Inverter::set_grid_power_phase_t_sensor(sensor::Sensor *grid_power_phase_t_sensor) { G3_dynamic.at(GRID_POWER_PHASE_T).sensor = grid_power_phase_t_sensor; }
		void SofarSolar_Inverter::set_off_grid_power_total_sensor(sensor::Sensor *off_grid_power_total_sensor) { G3_dynamic.at(OFF_GRID_POWER_TOTAL).sensor = off_grid_power_total_sensor; }
		void SofarSolar_Inverter::set_off_grid_frequency_sensor(sensor::Sensor *off_grid_frequency_sensor) { G3_dynamic.at(OFF_GRID_FREQUENCY).sensor = off_grid_frequency_sensor; }
		void SofarSolar_Inverter::set_off_grid_voltage_phase_r_sensor(sensor::Sensor *off_grid_voltage_phase_r_sensor) { G3_dynamic.at(OFF_GRID_VOLTAGE_PHASE_R).sensor = off_grid_voltage_phase_r_sensor; }
		void SofarSolar_Inverter::set_off_grid_current_phase_r_sensor(sensor::Sensor *off_grid_current_phase_r_sensor) { G3_dynamic.at(OFF_GRID_CURRENT_PHASE_R).sensor = off_grid_current_phase_r_sensor; }
		void SofarSolar_Inverter::set_off_grid_power_phase_r_sensor(sensor::Sensor *off_grid_power_phase_r_sensor) { G3_dynamic.at(OFF_GRID_POWER_PHASE_R).sensor = off_grid_power_phase_r_sensor; }
		void SofarSolar_Inverter::set_off_grid_voltage_phase_s_sensor(sensor::Sensor *off_grid_voltage_phase_s_sensor) { G3_dynamic.at(OFF_GRID_VOLTAGE_PHASE_S).sensor = off_grid_voltage_phase_s_sensor; }
		void SofarSolar_Inverter::set_off_grid_current_phase_s_sensor(sensor::Sensor *off_grid_current_phase_s_sensor) { G3_dynamic.at(OFF_GRID_CURRENT_PHASE_S).sensor = off_grid_current_phase_s_sensor; }
		void SofarSolar_Inverter::set_off_grid_power_phase_s_sensor(sensor::Sensor *off_grid_power_phase_s_sensor) { G3_dynamic.at(OFF_GRID_POWER_PHASE_S).sensor = off_grid_power_phase_s_sensor; }
		void SofarSolar_Inverter::set_off_grid_voltage_phase_t_sensor(sensor::Sensor *off_grid_voltage_phase_t_sensor) { G3_dynamic.at(OFF_GRID_VOLTAGE_PHASE_T).sensor = off_grid_voltage_phase_t_sensor; }
		void SofarSolar_Inverter::set_off_grid_current_phase_t_sensor(sensor::Sensor *off_grid_current_phase_t_sensor) { G3_dynamic.at(OFF_GRID_CURRENT_PHASE_T).sensor = off_grid_current_phase_t_sensor; }
		void SofarSolar_Inverter::set_off_grid_power_phase_t_sensor(sensor::Sensor *off_grid_power_phase_t_sensor) { G3_dynamic.at(OFF_GRID_POWER_PHASE_T).sensor = off_grid_power_phase_t_sensor; }
		void SofarSolar_Inverter::set_battery_active_control_sensor(sensor::Sensor *battery_active_control_sensor) { G3_dynamic.at(BATTERY_ACTIVE_CONTROL).sensor = battery_active_control_sensor; }
		void SofarSolar_Inverter::set_battery_active_oneshot_sensor(sensor::Sensor *battery_active_oneshot_sensor) { G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).sensor = battery_active_oneshot_sensor; }


       // Set update intervals for sensors

        void SofarSolar_Inverter::set_pv_generation_today_sensor_update_interval(uint16_t pv_generation_today_sensor_update_interval) { G3_dynamic.at(PV_GENERATION_TODAY).update_interval = pv_generation_today_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_generation_total_sensor_update_interval(uint16_t pv_generation_total_sensor_update_interval) { G3_dynamic.at(PV_GENERATION_TOTAL).update_interval = pv_generation_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_load_consumption_today_sensor_update_interval(uint16_t load_consumption_today_sensor_update_interval) { G3_dynamic.at(LOAD_CONSUMPTION_TODAY).update_interval = load_consumption_today_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_load_consumption_total_sensor_update_interval(uint16_t load_consumption_total_sensor_update_interval) { G3_dynamic.at(LOAD_CONSUMPTION_TOTAL).update_interval = load_consumption_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_charge_today_sensor_update_interval(uint16_t battery_charge_today_sensor_update_interval) { G3_dynamic.at(BATTERY_CHARGE_TODAY).update_interval = battery_charge_today_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_charge_total_sensor_update_interval(uint16_t battery_charge_total_sensor_update_interval) { G3_dynamic.at(BATTERY_CHARGE_TOTAL).update_interval = battery_charge_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_discharge_today_sensor_update_interval(uint16_t battery_discharge_today_sensor_update_interval) { G3_dynamic.at(BATTERY_DISCHARGE_TODAY).update_interval = battery_discharge_today_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_discharge_total_sensor_update_interval(uint16_t battery_discharge_total_sensor_update_interval) { G3_dynamic.at(BATTERY_DISCHARGE_TOTAL).update_interval = battery_discharge_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_total_active_power_inverter_sensor_update_interval(uint16_t total_active_power_inverter_update_interval) { G3_dynamic.at(TOTAL_ACTIVE_POWER_INVERTER).update_interval = total_active_power_inverter_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_voltage_1_sensor_update_interval(uint16_t pv_voltage_1_sensor_update_interval) { G3_dynamic.at(PV_VOLTAGE_1).update_interval = pv_voltage_1_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_current_1_sensor_update_interval(uint16_t pv_current_1_sensor_update_interval) { G3_dynamic.at(PV_CURRENT_1).update_interval = pv_current_1_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_power_1_sensor_update_interval(uint16_t pv_power_1_sensor_update_interval) { G3_dynamic.at(PV_POWER_1).update_interval = pv_power_1_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_voltage_2_sensor_update_interval(uint16_t pv_voltage_2_sensor_update_interval) { G3_dynamic.at(PV_VOLTAGE_2).update_interval = pv_voltage_2_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_current_2_sensor_update_interval(uint16_t pv_current_2_sensor_update_interval) { G3_dynamic.at(PV_CURRENT_2).update_interval = pv_current_2_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_power_2_sensor_update_interval(uint16_t pv_power_2_sensor_update_interval) { G3_dynamic.at(PV_POWER_2).update_interval = pv_power_2_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_pv_power_total_sensor_update_interval(uint16_t pv_power_total_sensor_update_interval) { G3_dynamic.at(PV_POWER_TOTAL).update_interval = pv_power_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_power_total_sensor_update_interval(uint16_t battery_power_total_sensor_update_interval) { G3_dynamic.at(BATTERY_POWER_TOTAL).update_interval = battery_power_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_state_of_charge_total_sensor_update_interval(uint16_t battery_state_of_charge_total_sensor_update_interval) { G3_dynamic.at(BATTERY_STATE_OF_CHARGE_TOTAL).update_interval = battery_state_of_charge_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_desired_grid_power_sensor_update_interval(uint16_t desired_grid_power_sensor_update_interval) { G3_dynamic.at(DESIRED_GRID_POWER).update_interval = desired_grid_power_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_minimum_battery_power_sensor_update_interval(uint16_t minimum_battery_power_sensor_update_interval) { G3_dynamic.at(MINIMUM_BATTERY_POWER).update_interval = minimum_battery_power_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor_update_interval(uint16_t maximum_battery_power_sensor_update_interval) { G3_dynamic.at(MAXIMUM_BATTERY_POWER).update_interval = maximum_battery_power_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor_update_interval(uint16_t energy_storage_mode_sensor_update_interval) { G3_dynamic.at(ENERGY_STORAGE_MODE).update_interval = energy_storage_mode_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor_update_interval(uint16_t battery_conf_id_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_ID).update_interval = battery_conf_id_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor_update_interval(uint16_t battery_conf_address_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_ADDRESS).update_interval = battery_conf_address_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor_update_interval(uint16_t battery_conf_protocol_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_PROTOCOL).update_interval = battery_conf_protocol_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_update_interval(uint16_t battery_conf_voltage_nominal_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).update_interval = battery_conf_voltage_nominal_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_update_interval(uint16_t battery_conf_voltage_over_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).update_interval = battery_conf_voltage_over_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_update_interval(uint16_t battery_conf_voltage_charge_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).update_interval = battery_conf_voltage_charge_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_update_interval(uint16_t battery_conf_voltage_lack_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).update_interval = battery_conf_voltage_lack_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_update_interval(uint16_t battery_conf_voltage_discharge_stop_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).update_interval = battery_conf_voltage_discharge_stop_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_update_interval(uint16_t battery_conf_current_charge_limit_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).update_interval = battery_conf_current_charge_limit_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_update_interval(uint16_t battery_conf_current_discharge_limit_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).update_interval = battery_conf_current_discharge_limit_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_update_interval(uint16_t battery_conf_depth_of_discharge_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).update_interval = battery_conf_depth_of_discharge_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_update_interval(uint16_t battery_conf_end_of_discharge_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).update_interval = battery_conf_end_of_discharge_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor_update_interval(uint16_t battery_conf_capacity_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_CAPACITY).update_interval = battery_conf_capacity_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_update_interval(uint16_t battery_conf_cell_type_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_CELL_TYPE).update_interval = battery_conf_cell_type_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_update_interval(uint16_t battery_conf_eps_buffer_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).update_interval = battery_conf_eps_buffer_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_conf_control_sensor_update_interval(uint16_t battery_conf_control_sensor_update_interval) { G3_dynamic.at(BATTERY_CONF_CONTROL).update_interval = battery_conf_control_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_frequency_sensor_update_interval(uint16_t grid_frequency_sensor_update_interval) { G3_dynamic.at(GRID_FREQUENCY).update_interval = grid_frequency_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_voltage_phase_r_sensor_update_interval(uint16_t grid_voltage_phase_r_sensor_update_interval) { G3_dynamic.at(GRID_VOLTAGE_PHASE_R).update_interval = grid_voltage_phase_r_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_current_phase_r_sensor_update_interval(uint16_t grid_current_phase_r_sensor_update_interval) { G3_dynamic.at(GRID_CURRENT_PHASE_R).update_interval = grid_current_phase_r_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_power_phase_r_sensor_update_interval(uint16_t grid_power_phase_r_sensor_update_interval) { G3_dynamic.at(GRID_POWER_PHASE_R).update_interval = grid_power_phase_r_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_voltage_phase_s_sensor_update_interval(uint16_t grid_voltage_phase_s_sensor_update_interval) { G3_dynamic.at(GRID_VOLTAGE_PHASE_S).update_interval = grid_voltage_phase_s_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_current_phase_s_sensor_update_interval(uint16_t grid_current_phase_s_sensor_update_interval) { G3_dynamic.at(GRID_CURRENT_PHASE_S).update_interval = grid_current_phase_s_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_power_phase_s_sensor_update_interval(uint16_t grid_power_phase_s_sensor_update_interval) { G3_dynamic.at(GRID_POWER_PHASE_S).update_interval = grid_power_phase_s_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_voltage_phase_t_sensor_update_interval(uint16_t grid_voltage_phase_t_sensor_update_interval) { G3_dynamic.at(GRID_VOLTAGE_PHASE_T).update_interval = grid_voltage_phase_t_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_current_phase_t_sensor_update_interval(uint16_t grid_current_phase_t_sensor_update_interval) { G3_dynamic.at(GRID_CURRENT_PHASE_T).update_interval = grid_current_phase_t_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_grid_power_phase_t_sensor_update_interval(uint16_t grid_power_phase_t_sensor_update_interval) { G3_dynamic.at(GRID_POWER_PHASE_T).update_interval = grid_power_phase_t_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_power_total_sensor_update_interval(uint16_t off_grid_power_total_sensor_update_interval) { G3_dynamic.at(OFF_GRID_POWER_TOTAL).update_interval = off_grid_power_total_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_frequency_sensor_update_interval(uint16_t off_grid_frequency_sensor_update_interval) { G3_dynamic.at(OFF_GRID_FREQUENCY).update_interval = off_grid_frequency_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_voltage_phase_r_sensor_update_interval(uint16_t off_grid_voltage_phase_r_sensor_update_interval) { G3_dynamic.at(OFF_GRID_VOLTAGE_PHASE_R).update_interval = off_grid_voltage_phase_r_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_current_phase_r_sensor_update_interval(uint16_t off_grid_current_phase_r_sensor_update_interval) { G3_dynamic.at(OFF_GRID_CURRENT_PHASE_R).update_interval = off_grid_current_phase_r_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_power_phase_r_sensor_update_interval(uint16_t off_grid_power_phase_r_sensor_update_interval) { G3_dynamic.at(OFF_GRID_POWER_PHASE_R).update_interval = off_grid_power_phase_r_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_voltage_phase_s_sensor_update_interval(uint16_t off_grid_voltage_phase_s_sensor_update_interval) { G3_dynamic.at(OFF_GRID_VOLTAGE_PHASE_S).update_interval = off_grid_voltage_phase_s_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_current_phase_s_sensor_update_interval(uint16_t off_grid_current_phase_s_sensor_update_interval) { G3_dynamic.at(OFF_GRID_CURRENT_PHASE_S).update_interval = off_grid_current_phase_s_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_power_phase_s_sensor_update_interval(uint16_t off_grid_power_phase_s_sensor_update_interval) { G3_dynamic.at(OFF_GRID_POWER_PHASE_S).update_interval = off_grid_power_phase_s_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_voltage_phase_t_sensor_update_interval(uint16_t off_grid_voltage_phase_t_sensor_update_interval) { G3_dynamic.at(OFF_GRID_VOLTAGE_PHASE_T).update_interval = off_grid_voltage_phase_t_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_current_phase_t_sensor_update_interval(uint16_t off_grid_current_phase_t_sensor_update_interval) { G3_dynamic.at(OFF_GRID_CURRENT_PHASE_T).update_interval = off_grid_current_phase_t_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_off_grid_power_phase_t_sensor_update_interval(uint16_t off_grid_power_phase_t_sensor_update_interval) { G3_dynamic.at(OFF_GRID_POWER_PHASE_T).update_interval = off_grid_power_phase_t_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_active_control_sensor_update_interval(uint16_t battery_active_control_sensor_update_interval) { G3_dynamic.at(BATTERY_ACTIVE_CONTROL).update_interval = battery_active_control_sensor_update_interval * 1000; }
		void SofarSolar_Inverter::set_battery_active_oneshot_sensor_update_interval(uint16_t battery_active_oneshot_sensor_update_interval) { G3_dynamic.at(BATTERY_ACTIVE_ONESHOT).update_interval = battery_active_oneshot_sensor_update_interval * 1000; }


        // Set default values for sensors

		void SofarSolar_Inverter::set_desired_grid_power_sensor_default_value(int64_t default_value) { G3_dynamic.at(DESIRED_GRID_POWER).default_value.int64_value = default_value; G3_dynamic.at(DESIRED_GRID_POWER).default_value_set = true; }
		void SofarSolar_Inverter::set_minimum_battery_power_sensor_default_value(int64_t default_value) { G3_dynamic.at(MINIMUM_BATTERY_POWER).default_value.int64_value = default_value; G3_dynamic.at(MINIMUM_BATTERY_POWER).default_value_set = true; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor_default_value(int64_t default_value) { G3_dynamic.at(MAXIMUM_BATTERY_POWER).default_value.int64_value = default_value; G3_dynamic.at(MAXIMUM_BATTERY_POWER).default_value_set = true; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor_default_value(int64_t default_value) { G3_dynamic.at(ENERGY_STORAGE_MODE).default_value.int64_value = default_value; G3_dynamic.at(ENERGY_STORAGE_MODE).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_ID).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_ID).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_ADDRESS).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_ADDRESS).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_PROTOCOL).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_PROTOCOL).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_default_value(float default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).default_value.int64_value = static_cast<int64_t>(default_value * 0.1); G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_default_value(float default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).default_value.int64_value = static_cast<int64_t>(default_value * 0.1); G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_default_value(float default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).default_value.int64_value = static_cast<int64_t>(default_value * 0.1); G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_default_value(float default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).default_value.int64_value = static_cast<int64_t>(default_value * 0.1); G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_default_value(float default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).default_value.int64_value = static_cast<int64_t>(default_value * 0.1); G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_default_value(float default_value) { G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).default_value.int64_value = static_cast<int64_t>(default_value * 0.01); G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_default_value(float default_value) { G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).default_value.int64_value = static_cast<int64_t>(default_value * 0.01); G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_CAPACITY).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_CAPACITY).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_CELL_TYPE).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_CELL_TYPE).default_value_set = true; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_default_value(int64_t default_value) { G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).default_value.int64_value = default_value; G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).default_value_set = true; }

		void SofarSolar_Inverter::set_desired_grid_power_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(DESIRED_GRID_POWER).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_minimum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(MINIMUM_BATTERY_POWER).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(MAXIMUM_BATTERY_POWER).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(ENERGY_STORAGE_MODE).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_ID).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_ADDRESS).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_PROTOCOL).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_NOMINAL).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_OVER).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_CHARGE).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_LACK).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_VOLTAGE_DISCHARGE_STOP).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_CURRENT_CHARGE_LIMIT).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_CURRENT_DISCHARGE_LIMIT).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_DEPTH_OF_DISCHARGE).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_END_OF_DISCHARGE).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_CAPACITY).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_CELL_TYPE).enforce_default_value = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_enforce_default_value(bool enforce_default_value) { G3_dynamic.at(BATTERY_CONF_EPS_BUFFER).enforce_default_value = enforce_default_value; }
	}
}