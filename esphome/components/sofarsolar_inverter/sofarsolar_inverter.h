#pragma once
#include "queue"
#include "vector"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
    namespace sofarsolar_inverter {

        class SofarSolar_Inverter : public Component {
        public:
            void setup() override;
            void loop() override;
            void dump_config() override;

            void calc_crc(std::vector<uint8_t> frame);
            bool check_crc(std::vector<uint8_t> frame);

            void set_model(std::string model) { this->model_ = model;}
            void set_modbus_address(int modbus_address) { this->modbus_address_ = modbus_address;}
            void set_zero_export(bool zero_export) { this->zero_export_ = zero_export;}
            void set_power_id(std::string power_id) { this->power_id_ = power_id;}

            void set_pv_generation_today_sensor(sensor::Sensor *pv_generation_today_sensor) { pv_generation_today_sensor_ = pv_generation_today_sensor; }
            void set_pv_generation_total_sensor(sensor::Sensor *pv_generation_total_sensor) { pv_generation_total_sensor_ = pv_generation_total_sensor; }
            void set_load_consumption_today_sensor(sensor::Sensor *load_consumption_today_sensor) { load_consumption_today_sensor_ = load_consumption_today_sensor; }
            void set_load_consumption_total_sensor(sensor::Sensor *load_consumption_total_sensor) { load_consumption_total_sensor_ = load_consumption_total_sensor; }
            void set_battery_charge_today_sensor(sensor::Sensor *battery_charge_today_sensor) { battery_charge_today_sensor_ = battery_charge_today_sensor; }
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

            void set_pv_generation_today_sensor_update_interval(int pv_generation_today_sensor_update_interval) { pv_generation_today_sensor_update_interval_ = pv_generation_today_sensor_update_interval; }
            void set_pv_generation_total_sensor_update_interval(int pv_generation_total_sensor_update_interval) { pv_generation_total_sensor_update_interval_ = pv_generation_total_sensor_update_interval; }
            void set_load_consumption_today_sensor_update_interval(int load_consumption_today_sensor_update_interval) { load_consumption_today_sensor_update_interval_ = load_consumption_today_sensor_update_interval; }
            void set_load_consumption_total_sensor_update_interval(int load_consumption_total_sensor_update_interval) { load_consumption_total_sensor_update_interval_ = load_consumption_total_sensor_update_interval; }
            void set_battery_charge_today_sensor_update_interval(int battery_charge_today_sensor_update_interval) { battery_charge_today_sensor_update_interval_ = battery_charge_today_sensor_update_interval; }
            void set_battery_charge_total_sensor_update_interval(int battery_charge_total_sensor_update_interval) { battery_charge_total_sensor_update_interval_ = battery_charge_total_sensor_update_interval; }
            void set_battery_discharge_today_sensor_update_interval(int battery_discharge_today_sensor_update_interval) { battery_discharge_today_sensor_update_interval_ = battery_discharge_today_sensor_update_interval; }
            void set_battery_discharge_total_sensor_update_interval(int battery_discharge_total_sensor_update_interval) { battery_discharge_total_sensor_update_interval_ = battery_discharge_total_sensor_update_interval; }
            void set_total_active_power_inverter_sensor_update_interval(int total_active_power_inverter_sensor_update_interval) { total_active_power_inverter_sensor_update_interval_ = total_active_power_inverter_sensor_update_interval; }
            void set_pv_voltage_1_sensor_update_interval(int pv_voltage_1_sensor_update_interval) { pv_voltage_1_sensor_update_interval_ = pv_voltage_1_sensor_update_interval; }
            void set_pv_current_1_sensor_update_interval(int pv_current_1_sensor_update_interval) { pv_current_1_sensor_update_interval_ = pv_current_1_sensor_update_interval; }
            void set_pv_power_1_sensor_update_interval(int pv_power_1_sensor_update_interval) { pv_power_1_sensor_update_interval_ = pv_power_1_sensor_update_interval; }
            void set_pv_voltage_2_sensor_update_interval(int pv_voltage_2_sensor_update_interval) { pv_voltage_2_sensor_update_interval_ = pv_voltage_2_sensor_update_interval; }
            void set_pv_current_2_sensor_update_interval(int pv_current_2_sensor_update_interval) { pv_current_2_sensor_update_interval_ = pv_current_2_sensor_update_interval; }
            void set_pv_power_2_sensor_update_interval(int pv_power_2_sensor_update_interval) { pv_power_2_sensor_update_interval_ = pv_power_2_sensor_update_interval; }
            void set_pv_power_total_sensor_update_interval(int pv_power_total_sensor_update_interval) { pv_power_total_sensor_update_interval_ = pv_power_total_sensor_update_interval; }
            void set_battery_power_total_sensor_update_interval(int battery_power_total_sensor_update_interval) { battery_power_total_sensor_update_interval_ = battery_power_total_sensor_update_interval; }
            void set_battery_state_of_charge_total_sensor_update_interval(int battery_state_of_charge_total_sensor_update_interval) { battery_state_of_charge_total_sensor_update_interval_ = battery_state_of_charge_total_sensor_update_interval; }
            void set_desired_grid_power_sensor_update_interval(int desired_grid_power_sensor_update_interval) { desired_grid_power_sensor_update_interval_ = desired_grid_power_sensor_update_interval; }
            void set_minimum_battery_power_sensor_update_interval(int minimum_battery_power_sensor_update_interval) { minimum_battery_power_sensor_update_interval_ = minimum_battery_power_sensor_update_interval; }
            void set_maximum_battery_power_sensor_update_interval(int maximum_battery_power_sensor_update_interval) { maximum_battery_power_sensor_update_interval_ = maximum_battery_power_sensor_update_interval; }
            void set_energy_storage_mode_sensor_update_interval(int energy_storage_mode_sensor_update_interval) { energy_storage_mode_sensor_update_interval_ = energy_storage_mode_sensor_update_interval; }
            void set_battery_conf_id_sensor_update_interval(int battery_conf_id_sensor_update_interval) { battery_conf_id_sensor_update_interval_ = battery_conf_id_sensor_update_interval; }
            void set_battery_conf_address_sensor_update_interval(int battery_conf_address_sensor_update_interval) { battery_conf_address_sensor_update_interval_ = battery_conf_address_sensor_update_interval; }
            void set_battery_conf_protocol_sensor_update_interval(int battery_conf_protocol_sensor_update_interval) { battery_conf_protocol_sensor_update_interval_ = battery_conf_protocol_sensor_update_interval; }
            void set_battery_conf_voltage_nominal_sensor_update_interval(int battery_conf_voltage_nominal_sensor_update_interval) { battery_conf_voltage_nominal_sensor_update_interval_ = battery_conf_voltage_nominal_sensor_update_interval; }
            void set_battery_conf_voltage_over_sensor_update_interval(int battery_conf_voltage_over_sensor_update_interval) { battery_conf_voltage_over_sensor_update_interval_ = battery_conf_voltage_over_sensor_update_interval; }
            void set_battery_conf_voltage_charge_sensor_update_interval(int battery_conf_voltage_charge_sensor_update_interval) { battery_conf_voltage_charge_sensor_update_interval_ = battery_conf_voltage_charge_sensor_update_interval; }
            void set_battery_conf_voltage_lack_sensor_update_interval(int battery_conf_voltage_lack_sensor_update_interval) { battery_conf_voltage_lack_sensor_update_interval_ = battery_conf_voltage_lack_sensor_update_interval; }
            void set_battery_conf_voltage_discharge_stop_sensor_update_interval(int battery_conf_voltage_discharge_stop_sensor_update_interval) { battery_conf_voltage_discharge_stop_sensor_update_interval_ = battery_conf_voltage_discharge_stop_sensor_update_interval; }
            void set_battery_conf_current_charge_limit_sensor_update_interval(int battery_conf_current_charge_limit_sensor_update_interval) { battery_conf_current_charge_limit_sensor_update_interval_ = battery_conf_current_charge_limit_sensor_update_interval; }
            void set_battery_conf_current_discharge_limit_sensor_update_interval(int battery_conf_current_discharge_limit_sensor_update_interval) { battery_conf_current_discharge_limit_sensor_update_interval_ = battery_conf_current_discharge_limit_sensor_update_interval; }
            void set_battery_conf_depth_of_discharge_sensor_update_interval(int battery_conf_depth_of_discharge_sensor_update_interval) { battery_conf_depth_of_discharge_sensor_update_interval_ = battery_conf_depth_of_discharge_sensor_update_interval; }
            void set_battery_conf_end_of_discharge_sensor_update_interval(int battery_conf_end_of_discharge_sensor_update_interval) { battery_conf_end_of_discharge_sensor_update_interval_ = battery_conf_end_of_discharge_sensor_update_interval; }
            void set_battery_conf_capacity_sensor_update_interval(int battery_conf_capacity_sensor_update_interval) { battery_conf_capacity_sensor_update_interval_ = battery_conf_capacity_sensor_update_interval; }
            void set_battery_conf_cell_type_sensor_update_interval(int battery_conf_cell_type_sensor_update_interval) { battery_conf_cell_type_sensor_update_interval_ = battery_conf_cell_type_sensor_update_interval; }
            void set_battery_conf_eps_buffer_sensor_update_interval(int battery_conf_eps_buffer_sensor_update_interval) { battery_conf_eps_buffer_sensor_update_interval_ = battery_conf_eps_buffer_sensor_update_interval; }
            void set_battery_conf_control_sensor_update_interval(int battery_conf_control_sensor_update_interval) { battery_conf_control_sensor_update_interval_ = battery_conf_control_sensor_update_interval; }

            std::string model_;
            int modbus_address_;
            bool zero_export_;
            std::string power_id_;

            sensor::Sensor *pv_generation_today_sensor_{nullptr};
            sensor::Sensor *pv_generation_total_sensor_{nullptr};
            sensor::Sensor *load_consumption_today_sensor_{nullptr};
            sensor::Sensor *load_consumption_total_sensor_{nullptr};
            sensor::Sensor *battery_charge_today_sensor_{nullptr};
            sensor::Sensor *battery_charge_total_sensor_{nullptr};
            sensor::Sensor *battery_discharge_today_sensor_{nullptr};
            sensor::Sensor *battery_discharge_total_sensor_{nullptr};
            sensor::Sensor *total_active_power_inverter_sensor_{nullptr};
            sensor::Sensor *pv_voltage_1_sensor_{nullptr};
            sensor::Sensor *pv_current_1_sensor_{nullptr};
            sensor::Sensor *pv_power_1_sensor_{nullptr};
            sensor::Sensor *pv_voltage_2_sensor_{nullptr};
            sensor::Sensor *pv_current_2_sensor_{nullptr};
            sensor::Sensor *pv_power_2_sensor_{nullptr};
            sensor::Sensor *pv_power_total_sensor_{nullptr};
            sensor::Sensor *battery_power_total_sensor_{nullptr};
            sensor::Sensor *battery_state_of_charge_total_sensor_{nullptr};
            sensor::Sensor *desired_grid_power_sensor_{nullptr};
            sensor::Sensor *minimum_battery_power_sensor_{nullptr};
            sensor::Sensor *maximum_battery_power_sensor_{nullptr};
            sensor::Sensor *energy_storage_mode_sensor_{nullptr};
            sensor::Sensor *battery_conf_id_sensor_{nullptr};
            sensor::Sensor *battery_conf_address_sensor_{nullptr};
            sensor::Sensor *battery_conf_protocol_sensor_{nullptr};
            sensor::Sensor *battery_conf_voltage_nominal_sensor_{nullptr};
            sensor::Sensor *battery_conf_voltage_over_sensor_{nullptr};
            sensor::Sensor *battery_conf_voltage_charge_sensor_{nullptr};
            sensor::Sensor *battery_conf_voltage_lack_sensor_{nullptr};
            sensor::Sensor *battery_conf_voltage_discharge_stop_sensor_{nullptr};
            sensor::Sensor *battery_conf_current_charge_limit_sensor_{nullptr};
            sensor::Sensor *battery_conf_current_discharge_limit_sensor_{nullptr};
            sensor::Sensor *battery_conf_depth_of_discharge_sensor_{nullptr};
            sensor::Sensor *battery_conf_end_of_discharge_sensor_{nullptr};
            sensor::Sensor *battery_conf_capacity_sensor_{nullptr};
            sensor::Sensor *battery_conf_cell_type_sensor_{nullptr};
            sensor::Sensor *battery_conf_eps_buffer_sensor_{nullptr};
            sensor::Sensor *battery_conf_control_sensor_{nullptr};

            int pv_generation_today_sensor_update_interval_;
            int pv_generation_total_sensor_update_interval_;
            int load_consumption_today_sensor_update_interval_;
            int load_consumption_total_sensor_update_interval_;
            int battery_charge_today_sensor_update_interval_;
            int battery_charge_total_sensor_update_interval_;
            int battery_discharge_today_sensor_update_interval_;
            int battery_discharge_total_sensor_update_interval_;
            int total_active_power_inverter_sensor_update_interval_;
            int pv_voltage_1_sensor_update_interval_;
            int pv_current_1_sensor_update_interval_;
            int pv_power_1_sensor_update_interval_;
            int pv_voltage_2_sensor_update_interval_;
            int pv_current_2_sensor_update_interval_;
            int pv_power_2_sensor_update_interval_;
            int pv_power_total_sensor_update_interval_;
            int battery_power_total_sensor_update_interval_;
            int battery_state_of_charge_total_sensor_update_interval_;
            int desired_grid_power_sensor_update_interval_;
            int minimum_battery_power_sensor_update_interval_;
            int maximum_battery_power_sensor_update_interval_;
            int energy_storage_mode_sensor_update_interval_;
            int battery_conf_id_sensor_update_interval_;
            int battery_conf_address_sensor_update_interval_;
            int battery_conf_protocol_sensor_update_interval_;
            int battery_conf_voltage_nominal_sensor_update_interval_;
            int battery_conf_voltage_over_sensor_update_interval_;
            int battery_conf_voltage_charge_sensor_update_interval_;
            int battery_conf_voltage_lack_sensor_update_interval_;
            int battery_conf_voltage_discharge_stop_sensor_update_interval_;
            int battery_conf_current_charge_limit_sensor_update_interval_;
            int battery_conf_current_discharge_limit_sensor_update_interval_;
            int battery_conf_depth_of_discharge_sensor_update_interval_;
            int battery_conf_end_of_discharge_sensor_update_interval_;
            int battery_conf_capacity_sensor_update_interval_;
            int battery_conf_cell_type_sensor_update_interval_;
            int battery_conf_eps_buffer_sensor_update_interval_;
            int battery_conf_control_sensor_update_interval_;

            static const uint16_t SofarSolar_Register[][5] = {
                // Register Address, Quantity, Type, Offset, Timer, Priority
                // Type: 0 uint16_t, 1 int16_t, 2 uint32_t, 3 int32_t, 4 float16, 5 float32
                // Priority: 0 - Low, 1 - Medium, 2 - High, 3 - Critical
                {0x0684, 2, 2, 0, 1}, // PV Generation Today
                {0x0686, 2, 2, 0, 0}, // PV Generation Total
                {0x0688, 2, 2, 0, 1}, // Load Consumption Today
                {0x068A, 2, 2, 0, 0}, // Load Consumption Total
                {0x0694, 2, 2, 0, 1}, // Battery Charge Today
                {0x0696, 2, 2, 0, 0}, // Battery Charge Total
                {0x0698, 2, 2, 0, 1}, // Battery Discharge Today
                {0x069A, 2, 2, 0, 0}, // Battery Discharge Total
                {0x0485, 1, 1, 0, 3}, // Total Active Power Inverter
                {0x0584, 1, 0, 0, 2}, // PV Voltage 1
                {0x0585, 1, 0, 0, 2}, // PV Current 1
                {0x0586, 1, 0, 0, 2}, // PV Power 1
                {0x0588, 1, 0, 0, 2}, // PV Voltage 2
                {0x0589, 1, 0, 0, 2}, // PV Current 2
                {0x058A, 1, 0, 0, 2}, // PV Power 2
                {0x05C4, 1, 0, 0, 3}, // PV Power Total
                {0x0667, 1, 1, 0, 3}, // Battery Power Total
                {0x0668, 1, 0, 0, 1}, // Battery State of Charge Total
                {0x1187, 2, 3, 0, 3}, // Desired Grid Power
                {0x1189, 2, 3, 0, 3}, // Minimum Battery Power
                {0x118B, 2, 3, 0, 3}, // Maximum Battery Power
                {0x1110, 1, 0, 0, 0}, // Energy Storage Mode
                {0x1044, 1, 0, 0, 0}, // Battery Conf ID
                {0x1045, 1, 0, 0, 0}, // Battery Conf Address
                {0x1046, 1, 0, 0, 0}, // Battery Conf Protocol
                {0x1050, 1, 0, 0, 0}, // Battery Conf Voltage Nominal
                {0x1047, 1, 0, 0, 0}, // Battery Conf Voltage Over
                {0x1048, 1, 0, 0, 0}, // Battery Conf Voltage Charge
                {0x1049, 1, 0, 0, 0}, // Battery Conf Voltage Lack
                {0x104A, 1, 0, 0, 0}, // Battery Conf Voltage Discharge Stop
                {0x104B, 1, 0, 0, 0}, // Battery Conf Current Charge Limit
                {0x104C, 1, 0, 0, 0}, // Battery Conf Current Discharge Limit
                {0x104D, 1, 0, 0, 0}, // Battery Conf Depth of Discharge
                {0x104E, 1, 0, 0, 0}, // Battery Conf End of Discharge
                {0x104F, 1, 0, 0, 0}, // Battery Conf Capacity
                {0x1051, 1, 0, 0, 0}, // Battery Conf Cell Type
                {0x1052, 1, 0, 0, 0}, // Battery Conf EPS Buffer
                {0x1053, 1, 0, 0, 0} // Battery Conf Control
            };
        };
    }
}