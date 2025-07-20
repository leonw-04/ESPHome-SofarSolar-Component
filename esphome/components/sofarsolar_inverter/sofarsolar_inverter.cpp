#include "queue"
#include "sofarsolar_inverter.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
    namespace sofarsolar_inverter {
        static const char *TAG = "sofarsolar_inverter.component";

		struct SofarSolar_RegisterDynamic {
			uint64_t last_update; // Last update time in milliseconds
			uint16_t update_interval; // Update interval in milliseconds
			sensor::Sensor *sensor; // Pointer to the sensor associated with the register
            SofarSolar_RegisterValue default_value; // Value of the register
            bool default_value_set; // Flag to indicate if the default value is set
			bool enforce_default_value; // Flag to indicate if the default value should be enforced
			bool is_queued = false; // Flag to indicate if the register is queued for reading/writing
			SofarSolar_RegisterDynamic(sensor::Sensor *sensor, uint16_t update_interval, SofarSolar_RegisterValue default_value = {}, bool default_value_set = false, bool enforce_default_value = false)
                : last_update(0), update_interval(update_interval), sensor(sensor), default_value(default_value), default_value_set(default_value_set), enforce_default_value(enforce_default_value) {}
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
            bool operator<(const register_write_task &other) const {
                return G3_registers.at(this->first_register_key).priority > G3_registers.at(other.first_register_key).priority;
            }
        };

        SofarSolar_Inverter::SofarSolar_Inverter() {
			this->G3_dynamic = {
				{PV_GENERATION_TODAY, SofarSolar_RegisterDynamic{pv_generation_today_sensor_, pv_generation_today_sensor_update_interval_, {}, false, false}}, // PV Generation Today
        	    {PV_GENERATION_TOTAL, SofarSolar_RegisterDynamic{pv_generation_total_sensor_, pv_generation_total_sensor_update_interval_, {}, false, false}}, // PV Generation Total
    	        {LOAD_CONSUMPTION_TODAY, SofarSolar_RegisterDynamic{load_consumption_today_sensor_, load_consumption_today_sensor_update_interval_, {}, false, false}}, // Load Consumption Today
    			{LOAD_CONSUMPTION_TOTAL, SofarSolar_RegisterDynamic{load_consumption_total_sensor_, load_consumption_total_sensor_update_interval_, {}, false, false}}, // Load Consumption Total
            	{BATTERY_CHARGE_TODAY, SofarSolar_RegisterDynamic{battery_charge_today_sensor_, battery_charge_today_sensor_update_interval_, {}, false, false}}, // Battery Charge Today
        	    {BATTERY_CHARGE_TOTAL, SofarSolar_RegisterDynamic{battery_charge_total_sensor_, battery_charge_total_sensor_update_interval_, {}, false, false}}, // Battery Charge Total
    	        {BATTERY_DISCHARGE_TODAY, SofarSolar_RegisterDynamic{battery_discharge_today_sensor_, battery_discharge_today_sensor_update_interval_, {}, false, false}}, // Battery Discharge Today
	            {BATTERY_DISCHARGE_TOTAL, SofarSolar_RegisterDynamic{battery_discharge_total_sensor_ ,battery_discharge_total_sensor_update_interval_ ,{}, false ,false}}, // Battery Discharge Total
            	{TOTAL_ACTIVE_POWER_INVERTER, SofarSolar_RegisterDynamic{total_active_power_inverter_sensor_ ,total_active_power_inverter_sensor_update_interval_ ,{}, false ,false}}, // Total Active Power Inverter
        	    {PV_VOLTAGE_1, SofarSolar_RegisterDynamic{pv_voltage_1_sensor_ ,pv_voltage_1_sensor_update_interval_ ,{}, false ,false}}, // PV Voltage 1
    	        {PV_CURRENT_1, SofarSolar_RegisterDynamic{pv_current_1_sensor_ ,pv_current_1_sensor_update_interval_ ,{}, false ,false}}, // PV Current 1
	            {PV_POWER_1, SofarSolar_RegisterDynamic{pv_power_1_sensor_ ,pv_power_1_sensor_update_interval_ ,{}, false ,false}}, // PV Power 1
            	{PV_VOLTAGE_2, SofarSolar_RegisterDynamic{pv_voltage_2_sensor_ ,pv_voltage_2_sensor_update_interval_ ,{}, false ,false}}, // PV Voltage 2
        	    {PV_CURRENT_2, SofarSolar_RegisterDynamic{pv_current_2_sensor_ ,pv_current_2_sensor_update_interval_ ,{}, false ,false}}, // PV Current 2
    	        {PV_POWER_2, SofarSolar_RegisterDynamic{pv_power_2_sensor_, pv_power_2_sensor_update_interval_, {}, false, false}}, // PV Power 2
	            {PV_POWER_TOTAL, SofarSolar_RegisterDynamic{pv_power_total_sensor_, pv_power_total_sensor_update_interval_, {}, false, false}}, // PV Power Total
            	{BATTERY_POWER_TOTAL, SofarSolar_RegisterDynamic{battery_power_total_sensor_, battery_power_total_sensor_update_interval_, {}, false, false}}, // Battery Power Total
        	    {BATTERY_STATE_OF_CHARGE_TOTAL, SofarSolar_RegisterDynamic{battery_state_of_charge_total_sensor_, battery_state_of_charge_total_sensor_update_interval_, {}, false, false}}, // Battery State of Charge Total
    	        {DESIRED_GRID_POWER, SofarSolar_RegisterDynamic{desired_grid_power_sensor_, desired_grid_power_sensor_update_interval_, desired_grid_power_sensor_default_value_, desired_grid_power_sensor_default_value_set_, desired_grid_power_sensor_enforce_default_value_}}, // Desired Grid Power
				{MINIMUM_BATTERY_POWER, SofarSolar_RegisterDynamic{minimum_battery_power_sensor_, minimum_battery_power_sensor_update_interval_, minimum_battery_power_sensor_default_value_, minimum_battery_power_sensor_default_value_set_, minimum_battery_power_sensor_enforce_default_value_}}, // Minimum Battery Power
				{MAXIMUM_BATTERY_POWER, SofarSolar_RegisterDynamic{maximum_battery_power_sensor_, maximum_battery_power_sensor_update_interval_, maximum_battery_power_sensor_default_value_, maximum_battery_power_sensor_default_value_set_, maximum_battery_power_sensor_enforce_default_value_}}, // Maximum Battery Power
				{ENERGY_STORAGE_MODE, SofarSolar_RegisterDynamic{energy_storage_mode_sensor_, energy_storage_mode_sensor_update_interval_, energy_storage_mode_sensor_default_value_, energy_storage_mode_sensor_default_value_set_, energy_storage_mode_sensor_enforce_default_value_}}, // Energy Storage Mode
				{BATTERY_CONF_ID, SofarSolar_RegisterDynamic{battery_conf_id_sensor_, battery_conf_id_sensor_update_interval_, battery_conf_id_sensor_default_value_, battery_conf_id_sensor_default_value_set_, battery_conf_id_sensor_enforce_default_value_}}, // Battery Conf ID
				{BATTERY_CONF_ADDRESS, SofarSolar_RegisterDynamic{battery_conf_address_sensor_, battery_conf_address_sensor_update_interval_, battery_conf_address_sensor_default_value_, battery_conf_address_sensor_default_value_set_, battery_conf_address_sensor_enforce_default_value_}}, // Battery Conf Address
				{BATTERY_CONF_PROTOCOL, SofarSolar_RegisterDynamic{battery_conf_protocol_sensor_, battery_conf_protocol_sensor_update_interval_, battery_conf_protocol_sensor_default_value_, battery_conf_protocol_sensor_default_value_set_, battery_conf_protocol_sensor_enforce_default_value_}}, // Battery Conf Protocol
				{BATTERY_CONF_VOLTAGE_NOMINAL, SofarSolar_RegisterDynamic{battery_conf_voltage_nominal_sensor_, battery_conf_voltage_nominal_sensor_update_interval_, battery_conf_voltage_nominal_sensor_default_value_, battery_conf_voltage_nominal_sensor_default_value_set_, battery_conf_voltage_nominal_sensor_enforce_default_value_}}, // Battery Conf Voltage Nominal
				{BATTERY_CONF_VOLTAGE_OVER, SofarSolar_RegisterDynamic{battery_conf_voltage_over_sensor_, battery_conf_voltage_over_sensor_update_interval_, battery_conf_voltage_over_sensor_default_value_, battery_conf_voltage_over_sensor_default_value_set_, battery_conf_voltage_over_sensor_enforce_default_value_}}, // Battery Conf Voltage Over
				{BATTERY_CONF_VOLTAGE_CHARGE, SofarSolar_RegisterDynamic{battery_conf_voltage_charge_sensor_, battery_conf_voltage_charge_sensor_update_interval_, battery_conf_voltage_charge_sensor_default_value_, battery_conf_voltage_charge_sensor_default_value_set_, battery_conf_voltage_charge_sensor_enforce_default_value_}}, // Battery Conf Voltage Charge
				{BATTERY_CONF_VOLTAGE_LACK, SofarSolar_RegisterDynamic{battery_conf_voltage_lack_sensor_ ,battery_conf_voltage_lack_sensor_update_interval_ , battery_conf_voltage_lack_sensor_default_value_, battery_conf_voltage_lack_sensor_default_value_set_ ,battery_conf_voltage_lack_sensor_enforce_default_value_}}, // Battery Conf Voltage Lack
				{BATTERY_CONF_VOLTAGE_DISCHARGE_STOP, SofarSolar_RegisterDynamic{battery_conf_voltage_discharge_stop_sensor_ ,battery_conf_voltage_discharge_stop_sensor_update_interval_ , battery_conf_voltage_discharge_stop_sensor_default_value_, battery_conf_voltage_discharge_stop_sensor_default_value_set_ ,battery_conf_voltage_discharge_stop_sensor_enforce_default_value_}}, // Battery Conf Voltage Discharge Stop
				{BATTERY_CONF_CURRENT_CHARGE_LIMIT, SofarSolar_RegisterDynamic{battery_conf_current_charge_limit_sensor_, battery_conf_current_charge_limit_sensor_update_interval_, battery_conf_current_charge_limit_sensor_default_value_, battery_conf_current_charge_limit_sensor_default_value_set_, battery_conf_current_charge_limit_sensor_enforce_default_value_}}, // Battery Conf Current Charge Limit
				{BATTERY_CONF_CURRENT_DISCHARGE_LIMIT, SofarSolar_RegisterDynamic{battery_conf_current_discharge_limit_sensor_, battery_conf_current_discharge_limit_sensor_update_interval_, battery_conf_current_discharge_limit_sensor_default_value_, battery_conf_current_discharge_limit_sensor_default_value_set_, battery_conf_current_discharge_limit_sensor_enforce_default_value_}}, // Battery Conf Current Discharge Limit
				{BATTERY_CONF_DEPTH_OF_DISCHARGE, SofarSolar_RegisterDynamic{battery_conf_depth_of_discharge_sensor_, battery_conf_depth_of_discharge_sensor_update_interval_, battery_conf_depth_of_discharge_sensor_default_value_, battery_conf_depth_of_discharge_sensor_default_value_set_, battery_conf_depth_of_discharge_sensor_enforce_default_value_}}, // Battery Conf Depth of Discharge
				{BATTERY_CONF_END_OF_DISCHARGE, SofarSolar_RegisterDynamic{battery_conf_end_of_discharge_sensor_ ,battery_conf_end_of_discharge_sensor_update_interval_ , battery_conf_end_of_discharge_sensor_default_value_, battery_conf_end_of_discharge_sensor_default_value_set_ ,battery_conf_end_of_discharge_sensor_enforce_default_value_}}, // Battery Conf End of Discharge
				{BATTERY_CONF_CAPACITY, SofarSolar_RegisterDynamic{battery_conf_capacity_sensor_ ,battery_conf_capacity_sensor_update_interval_, battery_conf_capacity_sensor_default_value_, battery_conf_capacity_sensor_default_value_set_ ,battery_conf_capacity_sensor_enforce_default_value_}}, // Battery Conf Capacity
				{BATTERY_CONF_CELL_TYPE, SofarSolar_RegisterDynamic{battery_conf_cell_type_sensor_, battery_conf_cell_type_sensor_update_interval_, battery_conf_cell_type_sensor_default_value_, battery_conf_cell_type_sensor_default_value_set_, battery_conf_cell_type_sensor_enforce_default_value_}}, // Battery Conf Cell Type
				{BATTERY_CONF_EPS_BUFFER, SofarSolar_RegisterDynamic{battery_conf_eps_buffer_sensor_, battery_conf_eps_buffer_sensor_update_interval_, battery_conf_eps_buffer_sensor_default_value_, battery_conf_eps_buffer_sensor_default_value_set_, battery_conf_eps_buffer_sensor_enforce_default_value_}}, // Battery Conf EPS Buffer
				{BATTERY_CONF_CONTROL, SofarSolar_RegisterDynamic{battery_conf_control_sensor_ ,battery_conf_control_sensor_update_interval_ ,{}, false ,false}}, // Battery Conf Control
				{GRID_FREQUENCY, SofarSolar_RegisterDynamic{grid_frequency_sensor_ ,grid_frequency_sensor_update_interval_ ,{}, false ,false}}, // Grid Frequency
				{GRID_VOLTAGE_PHASE_R, SofarSolar_RegisterDynamic{grid_voltage_phase_r_sensor_ ,grid_voltage_phase_r_sensor_update_interval_ ,{}, false ,false}}, // Grid Voltage Phase R
				{GRID_CURRENT_PHASE_R, SofarSolar_RegisterDynamic{grid_current_phase_r_sensor_, grid_current_phase_r_sensor_update_interval_, {}, false, false}}, // Grid Current Phase R
				{GRID_POWER_PHASE_R, SofarSolar_RegisterDynamic{grid_power_phase_r_sensor_, grid_power_phase_r_sensor_update_interval_, {}, false, false}}, // Grid Power Phase R
				{GRID_VOLTAGE_PHASE_S, SofarSolar_RegisterDynamic{grid_voltage_phase_s_sensor_, grid_voltage_phase_s_sensor_update_interval_, {}, false, false}}, // Grid Voltage Phase S
				{GRID_CURRENT_PHASE_S, SofarSolar_RegisterDynamic{grid_current_phase_s_sensor_, grid_current_phase_s_sensor_update_interval_, {}, false, false}}, // Grid Current Phase S
				{GRID_POWER_PHASE_S, SofarSolar_RegisterDynamic{grid_power_phase_s_sensor_, grid_power_phase_s_sensor_update_interval_, {}, false, false}}, // Grid Power Phase S
				{GRID_VOLTAGE_PHASE_T, SofarSolar_RegisterDynamic{grid_voltage_phase_t_sensor_, grid_voltage_phase_t_sensor_update_interval_, {}, false, false}}, // Grid Voltage Phase T
				{GRID_CURRENT_PHASE_T, SofarSolar_RegisterDynamic{grid_current_phase_t_sensor_, grid_current_phase_t_sensor_update_interval_, {}, false, false}}, // Grid Current Phase T
				{GRID_POWER_PHASE_T, SofarSolar_RegisterDynamic{grid_power_phase_t_sensor_, grid_power_phase_t_sensor_update_interval_, {}, false, false}}, // Grid Power Phase T
				{OFF_GRID_POWER_TOTAL, SofarSolar_RegisterDynamic{off_grid_power_total_sensor_ ,off_grid_power_total_sensor_update_interval_ ,{}, false ,false}}, // Off Grid Power Total
				{OFF_GRID_FREQUENCY, SofarSolar_RegisterDynamic{off_grid_frequency_sensor_, off_grid_frequency_sensor_update_interval_, {}, false, false}}, // Off Grid Frequency
				{OFF_GRID_VOLTAGE_PHASE_R, SofarSolar_RegisterDynamic{off_grid_voltage_phase_r_sensor_ ,off_grid_voltage_phase_r_sensor_update_interval_ ,{}, false ,false}}, // Off Grid Voltage Phase R
				{OFF_GRID_CURRENT_PHASE_R, SofarSolar_RegisterDynamic{off_grid_current_phase_r_sensor_ ,off_grid_current_phase_r_sensor_update_interval_ ,{}, false ,false}}, // Off Grid Current Phase R
				{OFF_GRID_POWER_PHASE_R, SofarSolar_RegisterDynamic{off_grid_power_phase_r_sensor_ ,off_grid_power_phase_r_sensor_update_interval_ ,{}, false ,false}}, // Off Grid Power Phase R
				{OFF_GRID_VOLTAGE_PHASE_S, SofarSolar_RegisterDynamic{off_grid_voltage_phase_s_sensor_, off_grid_voltage_phase_s_sensor_update_interval_, {}, false, false}}, // Off Grid Voltage Phase S
				{OFF_GRID_CURRENT_PHASE_S, SofarSolar_RegisterDynamic{off_grid_current_phase_s_sensor_, off_grid_current_phase_s_sensor_update_interval_, {}, false, false}}, // Off Grid Current Phase S
				{OFF_GRID_POWER_PHASE_S, SofarSolar_RegisterDynamic{off_grid_power_phase_s_sensor_, off_grid_power_phase_s_sensor_update_interval_, {}, false, false}}, // Off Grid Power Phase S
				{OFF_GRID_VOLTAGE_PHASE_T, SofarSolar_RegisterDynamic{off_grid_voltage_phase_t_sensor_ ,off_grid_voltage_phase_t_sensor_update_interval_ ,{}, false ,false}}, // Off Grid Voltage Phase T
				{OFF_GRID_CURRENT_PHASE_T, SofarSolar_RegisterDynamic{off_grid_current_phase_t_sensor_, off_grid_current_phase_t_sensor_update_interval_, {}, false, false}}, // Off Grid Current Phase T
				{OFF_GRID_POWER_PHASE_T, SofarSolar_RegisterDynamic{off_grid_power_phase_t_sensor_, off_grid_power_phase_t_sensor_update_interval_, {}, false, false}}, // Off Grid Power Phase T
				{BATTERY_ACTIVE_CONTROL, SofarSolar_RegisterDynamic{battery_active_control_sensor_, battery_active_control_sensor_update_interval_, {}, false, false}}, // Battery Active Control
				{BATTERY_ACTIVE_ONESHOT, SofarSolar_RegisterDynamic{battery_active_oneshot_sensor_, battery_active_oneshot_sensor_update_interval_, {}, false, false}} // Battery Active Oneshot
        	};
        }

        int time_last_loop = 0;
        bool current_reading = false; // Pointer to the current reading task
        bool current_writing = false; // Pointer to the current writing task
        register_write_task current_write_task; // Pointer to the current writing task
        uint64_t time_begin_modbus_operation = 0;
        uint64_t zero_export_last_update = 0;
        std::priority_queue<register_read_task> register_read_queue; // Priority queue for register read tasks
        std::priority_queue<register_write_task> register_write_queue; // Priority queue for register write tasks

        void SofarSolar_Inverter::setup() {
			//ESP_LOGCONFIG(TAG, "Setting up Sofar Solar Inverter");
            //registers_G3[BATTERY_ACTIVE_CONTROL].write_value.uint16_value = 0;
            //registers_G3[BATTERY_ACTIVE_CONTROL].write_set_value = true;
            //registers_G3[BATTERY_ACTIVE_ONESHOT].write_value.uint16_value = 1;
            //registers_G3[BATTERY_ACTIVE_ONESHOT].write_set_value = true;
            //time_begin_modbus_operation = millis();
			//register_write_task data;
            //data.register_ptr = &registers_G3[BATTERY_ACTIVE_CONTROL];
            //this->write_battery_active(data); // Write the battery active control register
            //current_writing = true; // Set the flag to indicate that a write is in progress
            //current_write_task = data; // Set the flag to indicate that a write is in progress
        }

        void SofarSolar_Inverter::loop() {
            for (auto &dynamic_register : this->G3_dynamic) {
                if (dynamic_register.second.sensor == nullptr) {
					continue; // Skip if the sensor pointer is null
				}
                if (millis() - dynamic_register.second.last_update >= dynamic_register.second.update_interval && !dynamic_register.second.is_queued) {
                    register_read_task task;
					task.register_key = dynamic_register.first; // Set the register key for the task
                    dynamic_register.second.is_queued = true; // Mark the register as queued
                    register_read_queue.push(task); // Add the task to the read queue
                    ESP_LOGVV(TAG, "Queued register %s for reading", dynamic_register.first.c_str());
                }
            }

			if (!current_reading && !register_read_queue.empty()) {
				read_modbus_registers(G3_registers.at(register_read_queue.top().register_key).start_address, G3_registers.at(register_read_queue.top().register_key).register_count);
			}
        }

		void SofarSolar_Inverter::on_modbus_data(const std::vector<uint8_t> &data) {
            ESP_LOGVV(TAG, "Received Modbus data: %s", vector_to_string(data).c_str());
			if (current_writing || current_reading) {
                switch (data[1]) {
					case 0x03: // Read Holding Registers
						parse_read_response(data);
						break;
					case 0x10: // Write Multiple Registers
						parse_write_response(data);
						break;
					default:
						ESP_LOGE(TAG, "Unknown Modbus function code: %02X", data[1]);
				}
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
                case U_WORD:
                    if (data.size() != 3 + 2 && data[2] != 2) {
                        ESP_LOGE(TAG, "Invalid read response size for U_WORD: %d", data.size());
                        return;
                    }
                    uint16_t value = (data[3] << 8) | data[4];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(value);
                    break;
				case S_WORD:
                    if (data.size() != 3 + 2 && data[2] != 2) {
                        ESP_LOGE(TAG, "Invalid read response size for INT_WORD: %d", data.size());
                        return;
                    }
                    int16_t value = (data[3] << 8) | data[4];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(value);
                    break;
				case U_DWORD:
                    if (data.size() != 3 + 4 && data[2] != 4) {
                        ESP_LOGE(TAG, "Invalid read response size for U_DWORD: %d", data.size());
                        return;
                    }
                    uint32_t value = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(value);
                    break;
				case S_DWORD:
                    if (data.size() != 3 + 4 && data[2] != 4) {
                        ESP_LOGE(TAG, "Invalid read response size for INT_DWORD: %d", data.size());
                        return;
                    }
                    int32_t value = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6];
                    G3_dynamic.at(register_read_queue.top().register_key).sensor->publish_state(value);
                    break;
				default:
                    ESP_LOGE(TAG, "Unsupported register type for read response: %d", G3_registers.at(register_read_queue.top().register_key).type);
                    return;
            }
        }


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
            } else if (registers_G3[MINIMUM_BATTERY_POWER].write_set_value) {
                new_minimum_battery_power = registers_G3[MINIMUM_BATTERY_POWER].write_value.int32_value;
            } else {
                new_minimum_battery_power = registers_G3[MINIMUM_BATTERY_POWER].sensor->state;
            }
            int32_t new_maximum_battery_power;
            if (registers_G3[MAXIMUM_BATTERY_POWER].enforce_default_value && registers_G3[DESIRED_GRID_POWER].is_default_value_set) {
                new_maximum_battery_power = registers_G3[MAXIMUM_BATTERY_POWER].default_value.int32_value;
            } else if (registers_G3[MAXIMUM_BATTERY_POWER].write_set_value) {
                new_maximum_battery_power = registers_G3[MAXIMUM_BATTERY_POWER].write_value.int32_value;
            } else {
                new_maximum_battery_power = registers_G3[MAXIMUM_BATTERY_POWER].sensor->state;
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

		void SofarSolar_Inverter::write_battery_active(register_write_task &task) {
            // Write the battery active state
            ESP_LOGD(TAG, "Writing battery active state");
            std::vector<uint8_t> data;
            uint16_t new_battery_active_control;
            if (registers_G3[BATTERY_ACTIVE_CONTROL].enforce_default_value && registers_G3[BATTERY_ACTIVE_CONTROL].is_default_value_set) {
                new_battery_active_control = registers_G3[BATTERY_ACTIVE_CONTROL].default_value.uint16_value;
            } else if (registers_G3[BATTERY_ACTIVE_CONTROL].write_set_value) {
                new_battery_active_control = registers_G3[BATTERY_ACTIVE_CONTROL].write_value.uint16_value;
            } else {
                new_battery_active_control = registers_G3[BATTERY_ACTIVE_CONTROL].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_active_control >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_active_control & 0xFF));
            uint16_t new_battery_active_oneshot;
            if (registers_G3[BATTERY_ACTIVE_ONESHOT].enforce_default_value && registers_G3[BATTERY_ACTIVE_ONESHOT].is_default_value_set) {
                new_battery_active_oneshot = registers_G3[BATTERY_ACTIVE_ONESHOT].default_value.uint16_value;
            } else if (registers_G3[BATTERY_ACTIVE_ONESHOT].write_set_value) {
                new_battery_active_oneshot = registers_G3[BATTERY_ACTIVE_ONESHOT].write_value.uint16_value;
            } else {
                new_battery_active_oneshot = registers_G3[BATTERY_ACTIVE_ONESHOT].sensor->state;
            }
            data.push_back(static_cast<uint8_t>(new_battery_active_oneshot >> 8));
            data.push_back(static_cast<uint8_t>(new_battery_active_oneshot & 0xFF));
            task.number_of_registers = (data.size() >> 1); // Number of registers to write
            send_write_modbus_registers(registers_G3[BATTERY_ACTIVE_CONTROL].start_address, task.number_of_registers, data);
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
            //for (const auto &reg : registers_G3) {
            //    ESP_LOGCONFIG(TAG, "  %s: start_address = %04X, type = %d, scale = %f, enforce_default_value = %s",
            //                  reg.second.sensor->get_name().c_str(), reg.second.start_address, reg.second.type, reg.second.scale,
            //                  TRUEFALSE(reg.second.enforce_default_value));
            //}
        }

        void SofarSolar_Inverter::read_modbus_registers(uint16_t start_address, uint16_t register_count) {
            // Create Modbus frame for reading registers
			std::vector<uint8_t> frame = {static_cast<uint8_t>(this->modbus_address_), 0x03, static_cast<uint8_t>(start_address >> 8), static_cast<uint8_t>(start_address & 0xFF), static_cast<uint8_t>(register_count >> 8), static_cast<uint8_t>(register_count & 0xFF)};
            this->send_raw(data);
        }

        void SofarSolar_Inverter::write_modbus_registers(uint16_t start_address, uint16_t register_count, const std::vector<uint8_t> &data) {
            // Create Modbus frame for writing registers
            std::vector<uint8_t> frame = {static_cast<uint8_t>(this->modbus_address_), 0x10, static_cast<uint8_t>(start_address >> 8), static_cast<uint8_t>(start_address & 0xFF), static_cast<uint8_t>(register_count >> 8), static_cast<uint8_t>(register_count & 0xFF), static_cast<uint8_t>(data.size())};
            frame.insert(frame.end(), data.begin(), data.end());
            this->send_raw(data);
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

        void SofarSolar_Inverter::set_pv_generation_today_sensor(sensor::Sensor *pv_generation_today_sensor) { pv_generation_today_sensor_ = pv_generation_today_sensor; }
        void SofarSolar_Inverter::set_pv_generation_total_sensor(sensor::Sensor *pv_generation_total_sensor) { pv_generation_total_sensor_ = pv_generation_total_sensor; }
        void SofarSolar_Inverter::set_load_consumption_today_sensor(sensor::Sensor *load_consumption_today_sensor) { load_consumption_today_sensor_ = load_consumption_today_sensor; }
        void SofarSolar_Inverter::set_load_consumption_total_sensor(sensor::Sensor *load_consumption_total_sensor) { load_consumption_total_sensor_ = load_consumption_total_sensor; }
        void SofarSolar_Inverter::set_battery_charge_today_sensor(sensor::Sensor *battery_charge_today_sensor) { battery_charge_today_sensor_ = battery_charge_today_sensor; }
        void SofarSolar_Inverter::set_battery_charge_total_sensor(sensor::Sensor *battery_charge_total_sensor) { battery_charge_total_sensor_ = battery_charge_total_sensor; }
        void SofarSolar_Inverter::set_battery_discharge_today_sensor(sensor::Sensor *battery_discharge_today_sensor) { battery_discharge_today_sensor_ = battery_discharge_today_sensor; }
        void SofarSolar_Inverter::set_battery_discharge_total_sensor(sensor::Sensor *battery_discharge_total_sensor) { battery_discharge_total_sensor_ = battery_discharge_total_sensor; }
        void SofarSolar_Inverter::set_total_active_power_inverter_sensor(sensor::Sensor *total_active_power_inverter_sensor) { total_active_power_inverter_sensor_ = total_active_power_inverter_sensor; }
        void SofarSolar_Inverter::set_pv_voltage_1_sensor(sensor::Sensor *pv_voltage_1_sensor) { pv_voltage_1_sensor_ = pv_voltage_1_sensor; }
        void SofarSolar_Inverter::set_pv_current_1_sensor(sensor::Sensor *pv_current_1_sensor) { pv_current_1_sensor_ = pv_current_1_sensor; }
        void SofarSolar_Inverter::set_pv_power_1_sensor(sensor::Sensor *pv_power_1_sensor) { pv_power_1_sensor_ = pv_power_1_sensor; }
        void SofarSolar_Inverter::set_pv_voltage_2_sensor(sensor::Sensor *pv_voltage_2_sensor) { pv_voltage_2_sensor_ = pv_voltage_2_sensor; }
        void SofarSolar_Inverter::set_pv_current_2_sensor(sensor::Sensor *pv_current_2_sensor) { pv_current_2_sensor_ = pv_current_2_sensor; }
        void SofarSolar_Inverter::set_pv_power_2_sensor(sensor::Sensor *pv_power_2_sensor) { pv_power_2_sensor_ = pv_power_2_sensor; }
        void SofarSolar_Inverter::set_pv_power_total_sensor(sensor::Sensor *pv_power_total_sensor) { pv_power_total_sensor_ = pv_power_total_sensor; }
        void SofarSolar_Inverter::set_battery_power_total_sensor(sensor::Sensor *battery_power_total_sensor) { battery_power_total_sensor_ = battery_power_total_sensor; }
        void SofarSolar_Inverter::set_battery_state_of_charge_total_sensor(sensor::Sensor *battery_state_of_charge_total_sensor) { battery_state_of_charge_total_sensor_ = battery_state_of_charge_total_sensor; }
        void SofarSolar_Inverter::set_desired_grid_power_sensor(sensor::Sensor *desired_grid_power_sensor) { desired_grid_power_sensor_ = desired_grid_power_sensor; }
        void SofarSolar_Inverter::set_minimum_battery_power_sensor(sensor::Sensor *minimum_battery_power_sensor) { minimum_battery_power_sensor_ = minimum_battery_power_sensor; }
        void SofarSolar_Inverter::set_maximum_battery_power_sensor(sensor::Sensor *maximum_battery_power_sensor) { maximum_battery_power_sensor_ = maximum_battery_power_sensor; }
        void SofarSolar_Inverter::set_energy_storage_mode_sensor(sensor::Sensor *energy_storage_mode_sensor) { energy_storage_mode_sensor_ = energy_storage_mode_sensor; }
        void SofarSolar_Inverter::set_battery_conf_id_sensor(sensor::Sensor *battery_conf_id_sensor) {battery_conf_id_sensor_ = battery_conf_id_sensor; }
        void SofarSolar_Inverter::set_battery_conf_address_sensor(sensor::Sensor *battery_conf_address_sensor) { battery_conf_address_sensor_ = battery_conf_address_sensor; }
        void SofarSolar_Inverter::set_battery_conf_protocol_sensor(sensor::Sensor *battery_conf_protocol_sensor) { battery_conf_protocol_sensor_ = battery_conf_protocol_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor(sensor::Sensor *battery_conf_voltage_over_sensor) { battery_conf_voltage_over_sensor_ = battery_conf_voltage_over_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor(sensor::Sensor *battery_conf_voltage_charge_sensor) { battery_conf_voltage_charge_sensor_ = battery_conf_voltage_charge_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor(sensor::Sensor *battery_conf_voltage_lack_sensor) { battery_conf_voltage_lack_sensor_ = battery_conf_voltage_lack_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor(sensor::Sensor *battery_conf_voltage_discharge_stop_sensor) { battery_conf_voltage_discharge_stop_sensor_ = battery_conf_voltage_discharge_stop_sensor; }
        void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor(sensor::Sensor *battery_conf_current_charge_limit_sensor) { battery_conf_current_charge_limit_sensor_ = battery_conf_current_charge_limit_sensor; }
        void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor(sensor::Sensor *battery_conf_current_discharge_limit_sensor) { battery_conf_current_discharge_limit_sensor_ = battery_conf_current_discharge_limit_sensor; }
        void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor(sensor::Sensor *battery_conf_depth_of_discharge_sensor) { battery_conf_depth_of_discharge_sensor_ = battery_conf_depth_of_discharge_sensor; }
        void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor(sensor::Sensor *battery_conf_end_of_discharge_sensor) { battery_conf_end_of_discharge_sensor_ = battery_conf_end_of_discharge_sensor; }
        void SofarSolar_Inverter::set_battery_conf_capacity_sensor(sensor::Sensor *battery_conf_capacity_sensor) { battery_conf_capacity_sensor_ = battery_conf_capacity_sensor; }
        void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor(sensor::Sensor *battery_conf_voltage_nominal_sensor) { battery_conf_voltage_nominal_sensor_ = battery_conf_voltage_nominal_sensor; }
        void SofarSolar_Inverter::set_battery_conf_cell_type_sensor(sensor::Sensor *battery_conf_cell_type_sensor) { battery_conf_cell_type_sensor_ = battery_conf_cell_type_sensor; }
        void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor(sensor::Sensor *battery_conf_eps_buffer_sensor) { battery_conf_eps_buffer_sensor_ = battery_conf_eps_buffer_sensor; }
        void SofarSolar_Inverter::set_battery_conf_control_sensor(sensor::Sensor *battery_conf_control_sensor) { battery_conf_control_sensor_ = battery_conf_control_sensor; }
        void SofarSolar_Inverter::set_grid_frequency_sensor(sensor::Sensor *grid_frequency_sensor) { grid_frequency_sensor_ = grid_frequency_sensor; }
        void SofarSolar_Inverter::set_grid_voltage_phase_r_sensor(sensor::Sensor *grid_voltage_phase_r_sensor) { grid_voltage_phase_r_sensor_ = grid_voltage_phase_r_sensor; }
        void SofarSolar_Inverter::set_grid_current_phase_r_sensor(sensor::Sensor *grid_current_phase_r_sensor) { grid_current_phase_r_sensor_ = grid_current_phase_r_sensor; }
        void SofarSolar_Inverter::set_grid_power_phase_r_sensor(sensor::Sensor *grid_power_phase_r_sensor) { grid_power_phase_r_sensor_ = grid_power_phase_r_sensor; }
        void SofarSolar_Inverter::set_grid_voltage_phase_s_sensor(sensor::Sensor *grid_voltage_phase_s_sensor) { grid_voltage_phase_s_sensor_ = grid_voltage_phase_s_sensor; }
        void SofarSolar_Inverter::set_grid_current_phase_s_sensor(sensor::Sensor *grid_current_phase_s_sensor) { grid_current_phase_s_sensor_ = grid_current_phase_s_sensor; }
        void SofarSolar_Inverter::set_grid_power_phase_s_sensor(sensor::Sensor *grid_power_phase_s_sensor) { grid_power_phase_s_sensor_ = grid_power_phase_s_sensor; }
        void SofarSolar_Inverter::set_grid_voltage_phase_t_sensor(sensor::Sensor *grid_voltage_phase_t_sensor) { grid_voltage_phase_t_sensor_ = grid_voltage_phase_t_sensor; }
        void SofarSolar_Inverter::set_grid_current_phase_t_sensor(sensor::Sensor *grid_current_phase_t_sensor) { grid_current_phase_t_sensor_ = grid_current_phase_t_sensor; }
        void SofarSolar_Inverter::set_grid_power_phase_t_sensor(sensor::Sensor *grid_power_phase_t_sensor) { grid_power_phase_t_sensor_ = grid_power_phase_t_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_total_sensor(sensor::Sensor *off_grid_power_total_sensor) { off_grid_power_total_sensor_ = off_grid_power_total_sensor; }
        void SofarSolar_Inverter::set_off_grid_frequency_sensor(sensor::Sensor *off_grid_frequency_sensor) { off_grid_frequency_sensor_ = off_grid_frequency_sensor; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_r_sensor(sensor::Sensor *off_grid_voltage_phase_r_sensor) { off_grid_voltage_phase_r_sensor_ = off_grid_voltage_phase_r_sensor; }
        void SofarSolar_Inverter::set_off_grid_current_phase_r_sensor(sensor::Sensor *off_grid_current_phase_r_sensor) { off_grid_current_phase_r_sensor_ = off_grid_current_phase_r_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_phase_r_sensor(sensor::Sensor *off_grid_power_phase_r_sensor) { off_grid_power_phase_r_sensor_ = off_grid_power_phase_r_sensor; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_s_sensor(sensor::Sensor *off_grid_voltage_phase_s_sensor) { off_grid_voltage_phase_s_sensor_ = off_grid_voltage_phase_s_sensor; }
        void SofarSolar_Inverter::set_off_grid_current_phase_s_sensor(sensor::Sensor *off_grid_current_phase_s_sensor) { off_grid_current_phase_s_sensor_ = off_grid_current_phase_s_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_phase_s_sensor(sensor::Sensor *off_grid_power_phase_s_sensor) { off_grid_power_phase_s_sensor_ = off_grid_power_phase_s_sensor; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_t_sensor(sensor::Sensor *off_grid_voltage_phase_t_sensor) { off_grid_voltage_phase_t_sensor_ = off_grid_voltage_phase_t_sensor; }
        void SofarSolar_Inverter::set_off_grid_current_phase_t_sensor(sensor::Sensor *off_grid_current_phase_t_sensor) { off_grid_current_phase_t_sensor_ = off_grid_current_phase_t_sensor; }
        void SofarSolar_Inverter::set_off_grid_power_phase_t_sensor(sensor::Sensor *off_grid_power_phase_t_sensor) { off_grid_power_phase_t_sensor_ = off_grid_power_phase_t_sensor; }
		void SofarSolar_Inverter::set_battery_active_control_sensor(sensor::Sensor *battery_active_control_sensor) { battery_active_control_sensor_ = battery_active_control_sensor; }
		void SofarSolar_Inverter::set_battery_active_oneshot_sensor(sensor::Sensor *battery_active_oneshot_sensor) { battery_active_oneshot_sensor_ = battery_active_oneshot_sensor; }
        // Set update intervals for sensors

        void SofarSolar_Inverter::set_pv_generation_today_sensor_update_interval(uint16_t pv_generation_today_sensor_update_interval) { pv_generation_today_sensor_update_interval_ = pv_generation_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_generation_total_sensor_update_interval(uint16_t pv_generation_total_sensor_update_interval) { pv_generation_total_sensor_update_interval_ = pv_generation_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_load_consumption_today_sensor_update_interval(uint16_t load_consumption_today_sensor_update_interval) { load_consumption_today_sensor_update_interval_ = load_consumption_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_load_consumption_total_sensor_update_interval(uint16_t load_consumption_total_sensor_update_interval) { load_consumption_total_sensor_update_interval_ = load_consumption_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_charge_today_sensor_update_interval(uint16_t battery_charge_today_sensor_update_interval) { battery_charge_today_sensor_update_interval_ = battery_charge_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_charge_total_sensor_update_interval(uint16_t battery_charge_total_sensor_update_interval) { battery_charge_total_sensor_update_interval_ = battery_charge_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_discharge_today_sensor_update_interval(uint16_t battery_discharge_today_sensor_update_interval) { battery_discharge_today_sensor_update_interval_ = battery_discharge_today_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_discharge_total_sensor_update_interval(uint16_t battery_discharge_total_sensor_update_interval) { battery_discharge_total_sensor_update_interval_ = battery_discharge_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_total_active_power_inverter_sensor_update_interval(uint16_t total_active_power_inverter_sensor_update_interval) { total_active_power_inverter_sensor_update_interval_ = total_active_power_inverter_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_voltage_1_sensor_update_interval(uint16_t pv_voltage_1_sensor_update_interval) { pv_voltage_1_sensor_update_interval_ = pv_voltage_1_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_current_1_sensor_update_interval(uint16_t pv_current_1_sensor_update_interval) { pv_current_1_sensor_update_interval_ = pv_current_1_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_power_1_sensor_update_interval(uint16_t pv_power_1_sensor_update_interval) { pv_power_1_sensor_update_interval_ = pv_power_1_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_voltage_2_sensor_update_interval(uint16_t pv_voltage_2_sensor_update_interval) { pv_voltage_2_sensor_update_interval_ = pv_voltage_2_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_current_2_sensor_update_interval(uint16_t pv_current_2_sensor_update_interval) { pv_current_2_sensor_update_interval_ = pv_current_2_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_power_2_sensor_update_interval(uint16_t pv_power_2_sensor_update_interval) { pv_power_2_sensor_update_interval_ = pv_power_2_sensor_update_interval; }
        void SofarSolar_Inverter::set_pv_power_total_sensor_update_interval(uint16_t pv_power_total_sensor_update_interval) { pv_power_total_sensor_update_interval_ = pv_power_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_power_total_sensor_update_interval(uint16_t battery_power_total_sensor_update_interval) { battery_power_total_sensor_update_interval_ = battery_power_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_state_of_charge_total_sensor_update_interval(uint16_t battery_state_of_charge_total_sensor_update_interval) { battery_state_of_charge_total_sensor_update_interval_ = battery_state_of_charge_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_desired_grid_power_sensor_update_interval(uint16_t desired_grid_power_sensor_update_interval) { desired_grid_power_sensor_update_interval_ = desired_grid_power_sensor_update_interval; }
        void SofarSolar_Inverter::set_minimum_battery_power_sensor_update_interval(uint16_t minimum_battery_power_sensor_update_interval) { minimum_battery_power_sensor_update_interval_ = minimum_battery_power_sensor_update_interval; }
        void SofarSolar_Inverter::set_maximum_battery_power_sensor_update_interval(uint16_t maximum_battery_power_sensor_update_interval) { maximum_battery_power_sensor_update_interval_ = maximum_battery_power_sensor_update_interval; }
        void SofarSolar_Inverter::set_energy_storage_mode_sensor_update_interval(uint16_t energy_storage_mode_sensor_update_interval) { energy_storage_mode_sensor_update_interval_ = energy_storage_mode_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_id_sensor_update_interval(uint16_t battery_conf_id_sensor_update_interval) { battery_conf_id_sensor_update_interval_ = battery_conf_id_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_address_sensor_update_interval(uint16_t battery_conf_address_sensor_update_interval) { battery_conf_address_sensor_update_interval_ = battery_conf_address_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_protocol_sensor_update_interval(uint16_t battery_conf_protocol_sensor_update_interval) { battery_conf_protocol_sensor_update_interval_ = battery_conf_protocol_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_update_interval(uint16_t battery_conf_voltage_nominal_sensor_update_interval) { battery_conf_voltage_nominal_sensor_update_interval_ = battery_conf_voltage_nominal_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_update_interval(uint16_t battery_conf_voltage_over_sensor_update_interval) { battery_conf_voltage_over_sensor_update_interval_ = battery_conf_voltage_over_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_update_interval(uint16_t battery_conf_voltage_charge_sensor_update_interval) { battery_conf_voltage_charge_sensor_update_interval_ = battery_conf_voltage_charge_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_update_interval(uint16_t battery_conf_voltage_lack_sensor_update_interval) { battery_conf_voltage_lack_sensor_update_interval_ = battery_conf_voltage_lack_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_update_interval(uint16_t battery_conf_voltage_discharge_stop_sensor_update_interval) { battery_conf_voltage_discharge_stop_sensor_update_interval_ = battery_conf_voltage_discharge_stop_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_update_interval(uint16_t battery_conf_current_charge_limit_sensor_update_interval) { battery_conf_current_charge_limit_sensor_update_interval_ = battery_conf_current_charge_limit_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_update_interval(uint16_t battery_conf_current_discharge_limit_sensor_update_interval) { battery_conf_current_discharge_limit_sensor_update_interval_ = battery_conf_current_discharge_limit_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_update_interval(uint16_t battery_conf_depth_of_discharge_sensor_update_interval) { battery_conf_depth_of_discharge_sensor_update_interval_ = battery_conf_depth_of_discharge_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_update_interval(uint16_t battery_conf_end_of_discharge_sensor_update_interval) { battery_conf_end_of_discharge_sensor_update_interval_ = battery_conf_end_of_discharge_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_capacity_sensor_update_interval(uint16_t battery_conf_capacity_sensor_update_interval) { battery_conf_capacity_sensor_update_interval_ = battery_conf_capacity_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_update_interval(uint16_t battery_conf_cell_type_sensor_update_interval) { battery_conf_cell_type_sensor_update_interval_ = battery_conf_cell_type_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_update_interval(uint16_t battery_conf_eps_buffer_sensor_update_interval) { battery_conf_eps_buffer_sensor_update_interval_ = battery_conf_eps_buffer_sensor_update_interval; }
        void SofarSolar_Inverter::set_battery_conf_control_sensor_update_interval(uint16_t battery_conf_control_sensor_update_interval) { battery_conf_control_sensor_update_interval_ = battery_conf_control_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_frequency_sensor_update_interval(uint16_t grid_frequency_sensor_update_interval) { grid_frequency_sensor_update_interval_ = grid_frequency_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_voltage_phase_r_sensor_update_interval(uint16_t grid_voltage_phase_r_sensor_update_interval) { grid_voltage_phase_r_sensor_update_interval_ = grid_voltage_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_current_phase_r_sensor_update_interval(uint16_t grid_current_phase_r_sensor_update_interval) { grid_current_phase_r_sensor_update_interval_ = grid_current_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_power_phase_r_sensor_update_interval(uint16_t grid_power_phase_r_sensor_update_interval) { grid_power_phase_r_sensor_update_interval_ = grid_power_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_voltage_phase_s_sensor_update_interval(uint16_t grid_voltage_phase_s_sensor_update_interval) { grid_voltage_phase_s_sensor_update_interval_ = grid_voltage_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_current_phase_s_sensor_update_interval(uint16_t grid_current_phase_s_sensor_update_interval) { grid_current_phase_s_sensor_update_interval_ = grid_current_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_power_phase_s_sensor_update_interval(uint16_t grid_power_phase_s_sensor_update_interval) { grid_power_phase_s_sensor_update_interval_ = grid_power_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_voltage_phase_t_sensor_update_interval(uint16_t grid_voltage_phase_t_sensor_update_interval) { grid_voltage_phase_t_sensor_update_interval_ = grid_voltage_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_current_phase_t_sensor_update_interval(uint16_t grid_current_phase_t_sensor_update_interval) { grid_current_phase_t_sensor_update_interval_ = grid_current_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_grid_power_phase_t_sensor_update_interval(uint16_t grid_power_phase_t_sensor_update_interval) { grid_power_phase_t_sensor_update_interval_ = grid_power_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_total_sensor_update_interval(uint16_t off_grid_power_total_sensor_update_interval) { off_grid_power_total_sensor_update_interval_ = off_grid_power_total_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_frequency_sensor_update_interval(uint16_t off_grid_frequency_sensor_update_interval) { off_grid_frequency_sensor_update_interval_ = off_grid_frequency_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_r_sensor_update_interval(uint16_t off_grid_voltage_phase_r_sensor_update_interval) { off_grid_voltage_phase_r_sensor_update_interval_ = off_grid_voltage_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_current_phase_r_sensor_update_interval(uint16_t off_grid_current_phase_r_sensor_update_interval) { off_grid_current_phase_r_sensor_update_interval_ = off_grid_current_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_phase_r_sensor_update_interval(uint16_t off_grid_power_phase_r_sensor_update_interval) { off_grid_power_phase_r_sensor_update_interval_ = off_grid_power_phase_r_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_s_sensor_update_interval(uint16_t off_grid_voltage_phase_s_sensor_update_interval) { off_grid_voltage_phase_s_sensor_update_interval_ = off_grid_voltage_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_current_phase_s_sensor_update_interval(uint16_t off_grid_current_phase_s_sensor_update_interval) { off_grid_current_phase_s_sensor_update_interval_ = off_grid_current_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_phase_s_sensor_update_interval(uint16_t off_grid_power_phase_s_sensor_update_interval) { off_grid_power_phase_s_sensor_update_interval_ = off_grid_power_phase_s_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_voltage_phase_t_sensor_update_interval(uint16_t off_grid_voltage_phase_t_sensor_update_interval) { off_grid_voltage_phase_t_sensor_update_interval_ = off_grid_voltage_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_current_phase_t_sensor_update_interval(uint16_t off_grid_current_phase_t_sensor_update_interval) { off_grid_current_phase_t_sensor_update_interval_ = off_grid_current_phase_t_sensor_update_interval; }
        void SofarSolar_Inverter::set_off_grid_power_phase_t_sensor_update_interval(uint16_t off_grid_power_phase_t_sensor_update_interval) { off_grid_power_phase_t_sensor_update_interval_ = off_grid_power_phase_t_sensor_update_interval; }
		void SofarSolar_Inverter::set_battery_active_control_sensor_update_interval(uint16_t battery_active_control_sensor_update_interval) { battery_active_control_sensor_update_interval_ = battery_active_control_sensor_update_interval; }
		void SofarSolar_Inverter::set_battery_active_oneshot_sensor_update_interval(uint16_t battery_active_oneshot_sensor_update_interval) { battery_active_oneshot_sensor_update_interval_ = battery_active_oneshot_sensor_update_interval; }


        void SofarSolar_Inverter::set_desired_grid_power_sensor_default_value(int64_t default_value) { desired_grid_power_sensor_default_value_.int64_value = default_value; desired_grid_power_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_minimum_battery_power_sensor_default_value(int64_t default_value) { minimum_battery_power_sensor_default_value_.int64_value = default_value; minimum_battery_power_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor_default_value(int64_t default_value) { maximum_battery_power_sensor_default_value_.int64_value = default_value; maximum_battery_power_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor_default_value(int64_t default_value) { energy_storage_mode_sensor_default_value_.int64_value = default_value; energy_storage_mode_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor_default_value(int64_t default_value) { battery_conf_id_sensor_default_value_.int64_value = default_value; battery_conf_id_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor_default_value(int64_t default_value) { battery_conf_address_sensor_default_value_.int64_value = default_value; battery_conf_address_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor_default_value(int64_t default_value) { battery_conf_protocol_sensor_default_value_.int64_value = default_value; battery_conf_protocol_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_default_value(float default_value) { battery_conf_voltage_nominal_sensor_default_value_.int64_value = static_cast<int64_t>(default_value * 0.1); battery_conf_voltage_nominal_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_default_value(float default_value) { battery_conf_voltage_over_sensor_default_value_.int64_value = static_cast<int64_t>(default_value * 0.1); battery_conf_voltage_over_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_default_value(float default_value) { battery_conf_voltage_charge_sensor_default_value_.int64_value = static_cast<int64_t>(default_value * 0.1); battery_conf_voltage_charge_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_default_value(float default_value) { battery_conf_voltage_lack_sensor_default_value_.int64_value = static_cast<int64_t>(default_value * 0.1); battery_conf_voltage_lack_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_default_value(float default_value) { battery_conf_voltage_discharge_stop_sensor_default_value_.int64_value = static_cast<int64_t>(default_value * 0.1); battery_conf_voltage_discharge_stop_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_default_value(float default_value) { battery_conf_current_charge_limit_sensor_default_value_.int64_value = static_cast<int64_t>(default_value * 0.01); battery_conf_current_charge_limit_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_default_value(float default_value) { battery_conf_current_discharge_limit_sensor_default_value_.int64_value = static_cast<int64_t>(default_value * 0.01); battery_conf_current_discharge_limit_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_default_value(int64_t default_value) { battery_conf_depth_of_discharge_sensor_default_value_.int64_value = default_value; battery_conf_depth_of_discharge_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_default_value(int64_t default_value) { battery_conf_end_of_discharge_sensor_default_value_.int64_value = default_value; battery_conf_end_of_discharge_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor_default_value(int64_t default_value) { battery_conf_capacity_sensor_default_value_.int64_value = default_value; battery_conf_capacity_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_default_value(int64_t default_value) { battery_conf_cell_type_sensor_default_value_.int64_value = default_value; battery_conf_cell_type_sensor_default_value_set_ = true; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_default_value(int64_t default_value) { battery_conf_eps_buffer_sensor_default_value_.int64_value = default_value; battery_conf_eps_buffer_sensor_default_value_set_ = true; }

		void SofarSolar_Inverter::set_desired_grid_power_sensor_enforce_default_value(bool enforce_default_value) { desired_grid_power_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_minimum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { minimum_battery_power_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_maximum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { maximum_battery_power_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_energy_storage_mode_sensor_enforce_default_value(bool enforce_default_value) { energy_storage_mode_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_id_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_id_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_address_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_address_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_protocol_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_protocol_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_nominal_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_voltage_nominal_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_over_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_voltage_over_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_charge_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_voltage_charge_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_lack_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_voltage_lack_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_voltage_discharge_stop_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_voltage_discharge_stop_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_current_charge_limit_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_current_charge_limit_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_current_discharge_limit_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_current_discharge_limit_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_depth_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_depth_of_discharge_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_end_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_end_of_discharge_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_capacity_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_capacity_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_cell_type_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_cell_type_sensor_enforce_default_value_ = enforce_default_value; }
		void SofarSolar_Inverter::set_battery_conf_eps_buffer_sensor_enforce_default_value(bool enforce_default_value) { battery_conf_eps_buffer_sensor_enforce_default_value_ = enforce_default_value;  }
    }
}