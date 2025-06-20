#pragma once

#include "esphome/core/component.h"

namespace esphome {
    namespace sofarsolar_inverter {

        class SofarSolar_Inverter : public Component {
        public:
            void setup() override;
            void update() override;
            void dump_config() override;

            void calc_crc(std::vector<uint8_t> frame);
            bool check_crc(std::vector<uint8_t> frame);

            void set_model(std::string model) { this->model_ = model;}
            void set_model(int modbus_address) { this->modbus_address_ = modbus_address;}
            void set_model(std::string uart_id) { this->uart_id_ = uart_id;}
            void set_model(bool zero_export) { this->zero_export_ = zero_export;}
            void set_model(std::string power_id) { this->power_id_ = power_id;}

            void set_pv_generation_today_sensor(sensor::Sensor *pv_generation_today_sensor) { pv_generation_today_sensor_ = pv_generation_today_sensor; }
            void set_pv_generation_total_sensor(sensor::Sensor *pv_generation_total_sensor) { pv_generation_total_sensor_ = pv_generation_total_sensor; }
            void set_load_consumption_today_sensor(sensor::Sensor *load_consumption_today_sensor) { load_consumption_today_sensor_ = load_consumption_today_sensor; }
            void set_load_consumption_total_sensor(sensor::Sensor *load_consumption_total_sensor) { load_consumption_total_sensor_ = load_consumption_total_sensor; }
            void set battery_charge_today_sensor(sensor::Sensor *battery_charge_today_sensor) { battery_charge_today_sensor_ = battery_charge_today_sensor; }
            void set_battery_charge_total_sensor(sensor::Sensor *battery_charge_total_sensor) { battery_charge_total_sensor_ = battery_charge_total_sensor; }
            void set_battery_discharge_today_sensor(sensor::Sensor *battery_discharge_today_sensor) { battery_discharge_today_sensor_ = battery_discharge_today_sensor; }
            void set_battery_discharge_total_sensor(sensor::Sensor *battery_discharge_total_sensor) { battery_discharge_total_sensor_ = battery_discharge_total_sensor; }
            void set_total_active_power_inverter_sensor(sensor::Sensor *total_active_power_inverter_sensor) { total_active_power_inverter_sensor_ = total_active_power_inverter_sensor; }
            void set_pv_voltage_1_sensor(sensor::Sensor *pv_voltage_1_sensor) { pv_voltage_1_sensor_ = pv_voltage_1_sensor; }
            void set_pv_current_1_sensor(sensor::Sensor *pv_current_1_sensor) { pv_current_1_sensor_ = pv_current_1_sensor; }
            void set_pv_power_1_sensor(sensor::Sensor *pv_power_1_sensor) { pv_power_1_sensor_ = pv_power_1_sensor; }
            void set_pv_voltage_2_sensor(sensor::Sensor *pv_voltage_2_sensor) { pv_voltage_2_sensor_ = pv_voltage_2_sensor; }
            void set_pv_current_2_sensor(sensor::Sensor *pv_current_2_sensor) { pv_current_2_sensor_ = pv_current_2_sensor; }
            void set_pv_power_2_sensor(sensor::Sensor *pv_power_2_sensor) { pv_power_2_sensor_ = pv_power_2_sensor; }
            void set_pv_power_total_sensor(sensor::Sensor *pv_power_total_sensor) { pv_power_total_sensor_ = pv_power_total_sensor; }
            void set_battery_power_total_sensor(sensor::Sensor *battery_power_total_sensor) { battery_power_total_sensor_ = battery_power_total_sensor; }
            void set_battery_state_of_charge_total_sensor(sensor::Sensor *battery_state_of_charge_total_sensor) { battery_state_of_charge_total_sensor_ = battery_state_of_charge_total_sensor; }
            void set_desired_grid_power_sensor(sensor::Sensor *desired_grid_power_sensor) { desired_grid_power_sensor_ = desired_grid_power_sensor; }
            void set_minimum_battery_power_sensor(sensor::Sensor *minimum_battery_power_sensor) { minimum_battery_power_sensor_ = minimum_battery_power_sensor; }
            void set_maximum_battery_power_sensor(sensor::Sensor *maximum_battery_power_sensor) { maximum_battery_power_sensor_ = maximum_battery_power_sensor; }
            void set_energy_storage_mode_sensor(sensor::Sensor *energy_storage_mode_sensor) { energy_storage_mode_sensor_ = energy_storage_mode_sensor; }
            void set_battery_conf_id_sensor(sensor::Sensor *battery_conf_id_sensor) { battery_conf_id_sensor_ = battery_conf_id_sensor; }
            void set_battery_conf_address_sensor(sensor::Sensor *battery_conf_address_sensor) { battery_conf_address_sensor_ = battery_conf_address_sensor; }
            void set_battery_conf_protocol_sensor(sensor::Sensor *battery_conf_protocol_sensor) { battery_conf_protocol_sensor_ = battery_conf_protocol_sensor; }
            void set_battery_conf_voltage_nominal_sensor(sensor::Sensor *battery_conf_voltage_nominal_sensor) { battery_conf_voltage_nominal_sensor_ = battery_conf_voltage_nominal_sensor; }
            void set_battery_conf_voltage_over_sensor(sensor::Sensor *battery_conf_voltage_over_sensor) { battery_conf_voltage_over_sensor_ = battery_conf_voltage_over_sensor; }
            void set_battery_conf_voltage_charge_sensor(sensor::Sensor *battery_conf_voltage_charge_sensor) { battery_conf_voltage_charge_sensor_ = battery_conf_voltage_charge_sensor; }
            void set_battery_conf_voltage_lack_sensor(sensor::Sensor *battery_conf_voltage_lack_sensor) { battery_conf_voltage_lack_sensor_ = battery_conf_voltage_lack_sensor; }
            void set_battery_conf_voltage_discharge_stop_sensor(sensor::Sensor *battery_conf_voltage_discharge_stop_sensor) { battery_conf_voltage_discharge_stop_sensor_ = battery_conf_voltage_discharge_stop_sensor; }
            void set_battery_conf_current_charge_limit_sensor(sensor::Sensor *battery_conf_current_charge_limit_sensor) { battery_conf_current_charge_limit_sensor_ = battery_conf_current_charge_limit_sensor; }
            void set_battery_conf_current_discharge_limit_sensor(sensor::Sensor *battery_conf_current_discharge_limit_sensor) { battery_conf_current_discharge_limit_sensor_ = battery_conf_current_discharge_limit_sensor; }
            void set_battery_conf_depth_of_discharge_sensor(sensor::Sensor *battery_conf_depth_of_discharge_sensor) { battery_conf_depth_of_discharge_sensor_ = battery_conf_depth_of_discharge_sensor; }
            void set_battery_conf_end_of_discharge_sensor(sensor::Sensor *battery_conf_end_of_discharge_sensor) { battery_conf_end_of_discharge_sensor_ = battery_conf_end_of_discharge_sensor; }
            void set_battery_conf_capacity_sensor(sensor::Sensor *battery_conf_capacity_sensor) { battery_conf_capacity_sensor_ = battery_conf_capacity_sensor; }
            void set_battery_conf_cell_type_sensor(sensor::Sensor *battery_conf_cell_type_sensor) { battery_conf_cell_type_sensor_ = battery_conf_cell_type_sensor; }
            void set_battery_conf_eps_buffer_sensor(sensor::Sensor *battery_conf_eps_buffer_sensor) { battery_conf_eps_buffer_sensor_ = battery_conf_eps_buffer_sensor; }
            void set_battery_conf_control_sensor(sensor::Sensor *battery_conf_control_sensor) { battery_conf_control_sensor_ = battery_conf_control_sensor; }

        }
    }
}