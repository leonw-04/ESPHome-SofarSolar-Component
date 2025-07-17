#pragma once
#include "queue"
#include "vector"
#include "map"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

#define PV_GENERATION_TODAY 1
#define PV_GENERATION_TOTAL 2
#define LOAD_CONSUMPTION_TODAY 3
#define LOAD_CONSUMPTION_TOTAL 4
#define BATTERY_CHARGE_TODAY 5
#define BATTERY_CHARGE_TOTAL 6
#define BATTERY_DISCHARGE_TODAY 7
#define BATTERY_DISCHARGE_TOTAL 8
#define TOTAL_ACTIVE_POWER_INVERTER 9
#define PV_VOLTAGE_1 10
#define PV_CURRENT_1 11
#define PV_POWER_1 12
#define PV_VOLTAGE_2 13
#define PV_CURRENT_2 14
#define PV_POWER_2 15
#define PV_POWER_TOTAL 16
#define BATTERY_POWER_TOTAL 17
#define BATTERY_STATE_OF_CHARGE_TOTAL 18
#define DESIRED_GRID_POWER 19
#define MINIMUM_BATTERY_POWER 20
#define MAXIMUM_BATTERY_POWER 21
#define ENERGY_STORAGE_MODE 22
#define BATTERY_CONF_ID 23
#define BATTERY_CONF_ADDRESS 24
#define BATTERY_CONF_PROTOCOL 25
#define BATTERY_CONF_VOLTAGE_NOMINAL 26
#define BATTERY_CONF_VOLTAGE_OVER 27
#define BATTERY_CONF_VOLTAGE_CHARGE 28
#define BATTERY_CONF_VOLTAGE_LACK 29
#define BATTERY_CONF_VOLTAGE_DISCHARGE_STOP 30
#define BATTERY_CONF_CURRENT_CHARGE_LIMIT 31
#define BATTERY_CONF_CURRENT_DISCHARGE_LIMIT 32
#define BATTERY_CONF_DEPTH_OF_DISCHARGE 33
#define BATTERY_CONF_END_OF_DISCHARGE 34
#define BATTERY_CONF_CAPACITY 35
#define BATTERY_CONF_CELL_TYPE 36
#define BATTERY_CONF_EPS_BUFFER 37
#define BATTERY_CONF_CONTROL 38
#define GRID_FREQUENCY 39
#define GRID_VOLTAGE_PHASE_R 40
#define GRID_CURRENT_PHASE_R 41
#define GRID_POWER_PHASE_R 42
#define GRID_VOLTAGE_PHASE_S 43
#define GRID_CURRENT_PHASE_S 44
#define GRID_POWER_PHASE_S 45
#define GRID_VOLTAGE_PHASE_T 46
#define GRID_CURRENT_PHASE_T 47
#define GRID_POWER_PHASE_T 48
#define OFF_GRID_POWER_TOTAL 49
#define OFF_GRID_FREQUENCY 50
#define OFF_GRID_VOLTAGE_PHASE_R 51
#define OFF_GRID_CURRENT_PHASE_R 52
#define OFF_GRID_POWER_PHASE_R 53
#define OFF_GRID_VOLTAGE_PHASE_S 54
#define OFF_GRID_CURRENT_PHASE_S 55
#define OFF_GRID_POWER_PHASE_S 56
#define OFF_GRID_VOLTAGE_PHASE_T 57
#define OFF_GRID_CURRENT_PHASE_T 58
#define OFF_GRID_POWER_PHASE_T 59

#define SINGLE_REGISTER_WRITE 1
#define DESIRED_GRID_POWER_WRITE 2
#define BATTERY_CONF_WRITE 3

namespace esphome {
    namespace sofarsolar_inverter {

		union SofarSolar_RegisterValue {
			uint16_t uint16_value;
			int16_t int16_value;
			uint32_t uint32_value;
			int32_t int32_value;
			uint64_t uint64_value;
		    int64_t int64_value;
		};

        struct SofarSolar_Register;

		struct register_read_task;

        struct register_write_task;


        class SofarSolar_Inverter : public uart::UARTDevice, public Component {
        public:

            std::map<uint8_t, SofarSolar_Register> registers_G3;

            SofarSolar_Inverter();

            void setup() override;
            void loop() override;
            void dump_config() override;

            void update_sensor(uint8_t register_index, std::vector<uint8_t> &response, uint8_t offset);
            void send_modbus(std::vector<uint8_t> frame);
            void calculate_crc(std::vector<uint8_t> &frame);
            bool check_crc(std::vector<uint8_t> frame);
            void send_read_modbus_registers(uint16_t start_address, uint16_t quantity);
            bool receive_modbus_response(std::vector<uint8_t> &response, uint8_t response_length);
            void send_write_modbus_registers(uint16_t start_address, uint16_t quantity, const std::vector<uint8_t> &data);
            void empty_uart_buffer();

            std::string vector_to_string(const std::vector<uint8_t> &data) {
                std::string result;
                for (const auto &byte : data) {
                    char buf[4];
                    std::sprintf(buf, "%02X ", byte);
                    result += buf;
                }
                return result;
            }
            bool check_for_response();
            bool read_response(std::vector<uint8_t> &response, SofarSolar_Register &register_info);
            uint64_t extract_data_from_response(std::vector<uint8_t> &response);
            bool write_response(SofarSolar_Register &register_info);
            bool check_for_error_code(std::vector<uint8_t> &response);

			void write_desired_grid_power(register_write_task &task);
			void write_battery_conf(register_write_task &task);
			void write_single_register(register_write_task &task);

            void set_model(std::string model) { this->model_ = model;}
            void set_modbus_address(int modbus_address) { this->modbus_address_ = modbus_address;}
            void set_zero_export(bool zero_export) { this->zero_export_ = zero_export;}
            void set_power_id(sensor::Sensor *power_id) { this->power_sensor_ = power_id;}

            void set_pv_generation_today_sensor(sensor::Sensor *pv_generation_today_sensor);
            void set_pv_generation_total_sensor(sensor::Sensor *pv_generation_total_sensor);
            void set_load_consumption_today_sensor(sensor::Sensor *load_consumption_today_sensor);
            void set_load_consumption_total_sensor(sensor::Sensor *load_consumption_total_sensor);
            void set_battery_charge_today_sensor(sensor::Sensor *battery_charge_today_sensor);
            void set_battery_charge_total_sensor(sensor::Sensor *battery_charge_total_sensor);
            void set_battery_discharge_today_sensor(sensor::Sensor *battery_discharge_today_sensor);
            void set_battery_discharge_total_sensor(sensor::Sensor *battery_discharge_total_sensor);
            void set_total_active_power_inverter_sensor(sensor::Sensor *total_active_power_inverter_sensor);
            void set_pv_voltage_1_sensor(sensor::Sensor *pv_voltage_1_sensor);
            void set_pv_current_1_sensor(sensor::Sensor *pv_current_1_sensor);
            void set_pv_power_1_sensor(sensor::Sensor *pv_power_1_sensor);
            void set_pv_voltage_2_sensor(sensor::Sensor *pv_voltage_2_sensor);
            void set_pv_current_2_sensor(sensor::Sensor *pv_current_2_sensor);
            void set_pv_power_2_sensor(sensor::Sensor *pv_power_2_sensor);
            void set_pv_power_total_sensor(sensor::Sensor *pv_power_total_sensor);
            void set_battery_power_total_sensor(sensor::Sensor *battery_power_total_sensor);
            void set_battery_state_of_charge_total_sensor(sensor::Sensor *battery_state_of_charge_total_sensor);
            void set_desired_grid_power_sensor(sensor::Sensor *desired_grid_power_sensor);
            void set_minimum_battery_power_sensor(sensor::Sensor *minimum_battery_power_sensor);
            void set_maximum_battery_power_sensor(sensor::Sensor *maximum_battery_power_sensor);
            void set_energy_storage_mode_sensor(sensor::Sensor *energy_storage_mode_sensor);
            void set_battery_conf_id_sensor(sensor::Sensor *battery_conf_id_sensor);
            void set_battery_conf_address_sensor(sensor::Sensor *battery_conf_address_sensor);
            void set_battery_conf_protocol_sensor(sensor::Sensor *battery_conf_protocol_sensor);
            void set_battery_conf_voltage_nominal_sensor(sensor::Sensor *battery_conf_voltage_nominal_sensor);
            void set_battery_conf_voltage_over_sensor(sensor::Sensor *battery_conf_voltage_over_sensor);
            void set_battery_conf_voltage_charge_sensor(sensor::Sensor *battery_conf_voltage_charge_sensor);
            void set_battery_conf_voltage_lack_sensor(sensor::Sensor *battery_conf_voltage_lack_sensor);
            void set_battery_conf_voltage_discharge_stop_sensor(sensor::Sensor *battery_conf_voltage_discharge_stop_sensor);
            void set_battery_conf_current_charge_limit_sensor(sensor::Sensor *battery_conf_current_charge_limit_sensor);
            void set_battery_conf_current_discharge_limit_sensor(sensor::Sensor *battery_conf_current_discharge_limit_sensor);
            void set_battery_conf_depth_of_discharge_sensor(sensor::Sensor *battery_conf_depth_of_discharge_sensor);
            void set_battery_conf_end_of_discharge_sensor(sensor::Sensor *battery_conf_end_of_discharge_sensor);
            void set_battery_conf_capacity_sensor(sensor::Sensor *battery_conf_capacity_sensor);
            void set_battery_conf_cell_type_sensor(sensor::Sensor *battery_conf_cell_type_sensor);
            void set_battery_conf_eps_buffer_sensor(sensor::Sensor *battery_conf_eps_buffer_sensor);
            void set_battery_conf_control_sensor(sensor::Sensor *battery_conf_control_sensor);
			void set_grid_frequency_sensor(sensor::Sensor *grid_frequency_sensor);
			void set_grid_voltage_phase_r_sensor(sensor::Sensor *grid_voltage_phase_r_sensor);
			void set_grid_current_phase_r_sensor(sensor::Sensor *grid_current_phase_r_sensor);
			void set_grid_power_phase_r_sensor(sensor::Sensor *grid_power_phase_r_sensor);
			void set_grid_voltage_phase_s_sensor(sensor::Sensor *grid_voltage_phase_s_sensor);
			void set_grid_current_phase_s_sensor(sensor::Sensor *grid_current_phase_s_sensor);
			void set_grid_power_phase_s_sensor(sensor::Sensor *grid_power_phase_s_sensor);
			void set_grid_voltage_phase_t_sensor(sensor::Sensor *grid_voltage_phase_t_sensor);
			void set_grid_current_phase_t_sensor(sensor::Sensor *grid_current_phase_t_sensor);
			void set_grid_power_phase_t_sensor(sensor::Sensor *grid_power_phase_t_sensor);
			void set_off_grid_power_total_sensor(sensor::Sensor *off_grid_power_total_sensor);
			void set_off_grid_frequency_sensor(sensor::Sensor *off_grid_frequency_sensor);
			void set_off_grid_voltage_phase_r_sensor(sensor::Sensor *off_grid_voltage_phase_r_sensor);
			void set_off_grid_current_phase_r_sensor(sensor::Sensor *off_grid_current_phase_r_sensor);
			void set_off_grid_power_phase_r_sensor(sensor::Sensor *off_grid_power_phase_r_sensor);
			void set_off_grid_voltage_phase_s_sensor(sensor::Sensor *off_grid_voltage_phase_s_sensor);
			void set_off_grid_current_phase_s_sensor(sensor::Sensor *off_grid_current_phase_s_sensor);
			void set_off_grid_power_phase_s_sensor(sensor::Sensor *off_grid_power_phase_s_sensor);
			void set_off_grid_voltage_phase_t_sensor(sensor::Sensor *off_grid_voltage_phase_t_sensor);
			void set_off_grid_current_phase_t_sensor(sensor::Sensor *off_grid_current_phase_t_sensor);
			void set_off_grid_power_phase_t_sensor(sensor::Sensor *off_grid_power_phase_t_sensor);

            // Set update intervals for sensors
            void set_pv_generation_today_sensor_update_interval(int pv_generation_today_sensor_update_interval);
            void set_pv_generation_total_sensor_update_interval(int pv_generation_total_sensor_update_interval);
            void set_load_consumption_today_sensor_update_interval(int load_consumption_today_sensor_update_interval);
            void set_load_consumption_total_sensor_update_interval(int load_consumption_total_sensor_update_interval);
            void set_battery_charge_today_sensor_update_interval(int battery_charge_today_sensor_update_interval);
            void set_battery_charge_total_sensor_update_interval(int battery_charge_total_sensor_update_interval);
            void set_battery_discharge_today_sensor_update_interval(int battery_discharge_today_sensor_update_interval);
            void set_battery_discharge_total_sensor_update_interval(int battery_discharge_total_sensor_update_interval);
            void set_total_active_power_inverter_sensor_update_interval(int total_active_power_inverter_sensor_update_interval);
            void set_pv_voltage_1_sensor_update_interval(int pv_voltage_1_sensor_update_interval);
            void set_pv_current_1_sensor_update_interval(int pv_current_1_sensor_update_interval);
            void set_pv_power_1_sensor_update_interval(int pv_power_1_sensor_update_interval);
            void set_pv_voltage_2_sensor_update_interval(int pv_voltage_2_sensor_update_interval);
            void set_pv_current_2_sensor_update_interval(int pv_current_2_sensor_update_interval);
            void set_pv_power_2_sensor_update_interval(int pv_power_2_sensor_update_interval);
            void set_pv_power_total_sensor_update_interval(int pv_power_total_sensor_update_interval);
            void set_battery_power_total_sensor_update_interval(int battery_power_total_sensor_update_interval);
            void set_battery_state_of_charge_total_sensor_update_interval(int battery_state_of_charge_total_sensor_update_interval);
            void set_desired_grid_power_sensor_update_interval(int desired_grid_power_sensor_update_interval);
            void set_minimum_battery_power_sensor_update_interval(int minimum_battery_power_sensor_update_interval);
            void set_maximum_battery_power_sensor_update_interval(int maximum_battery_power_sensor_update_interval);
            void set_energy_storage_mode_sensor_update_interval(int energy_storage_mode_sensor_update_interval);
            void set_battery_conf_id_sensor_update_interval(int battery_conf_id_sensor_update_interval);
            void set_battery_conf_address_sensor_update_interval(int battery_conf_address_sensor_update_interval);
            void set_battery_conf_protocol_sensor_update_interval(int battery_conf_protocol_sensor_update_interval);
            void set_battery_conf_voltage_nominal_sensor_update_interval(int battery_conf_voltage_nominal_sensor_update_interval);
            void set_battery_conf_voltage_over_sensor_update_interval(int battery_conf_voltage_over_sensor_update_interval);
            void set_battery_conf_voltage_charge_sensor_update_interval(int battery_conf_voltage_charge_sensor_update_interval);
            void set_battery_conf_voltage_lack_sensor_update_interval(int battery_conf_voltage_lack_sensor_update_interval);
            void set_battery_conf_voltage_discharge_stop_sensor_update_interval(int battery_conf_voltage_discharge_stop_sensor_update_interval);
            void set_battery_conf_current_charge_limit_sensor_update_interval(int battery_conf_current_charge_limit_sensor_update_interval);
            void set_battery_conf_current_discharge_limit_sensor_update_interval(int battery_conf_current_discharge_limit_sensor_update_interval);
            void set_battery_conf_depth_of_discharge_sensor_update_interval(int battery_conf_depth_of_discharge_sensor_update_interval);
            void set_battery_conf_end_of_discharge_sensor_update_interval(int battery_conf_end_of_discharge_sensor_update_interval);
            void set_battery_conf_capacity_sensor_update_interval(int battery_conf_capacity_sensor_update_interval);
            void set_battery_conf_cell_type_sensor_update_interval(int battery_conf_cell_type_sensor_update_interval);
            void set_battery_conf_eps_buffer_sensor_update_interval(int battery_conf_eps_buffer_sensor_update_interval);
            void set_battery_conf_control_sensor_update_interval(int battery_conf_control_sensor_update_interval);
			void set_grid_frequency_sensor_update_interval(int grid_frequency_sensor_update_interval);
			void set_grid_voltage_phase_r_sensor_update_interval(int grid_voltage_phase_r_sensor_update_interval);
			void set_grid_current_phase_r_sensor_update_interval(int grid_current_phase_r_sensor_update_interval);
			void set_grid_power_phase_r_sensor_update_interval(int grid_power_phase_r_sensor_update_interval);
			void set_grid_voltage_phase_s_sensor_update_interval(int grid_voltage_phase_s_sensor_update_interval);
			void set_grid_current_phase_s_sensor_update_interval(int grid_current_phase_s_sensor_update_interval);
			void set_grid_power_phase_s_sensor_update_interval(int grid_power_phase_s_sensor_update_interval);
			void set_grid_voltage_phase_t_sensor_update_interval(int grid_voltage_phase_t_sensor_update_interval);
			void set_grid_current_phase_t_sensor_update_interval(int grid_current_phase_t_sensor_update_interval);
			void set_grid_power_phase_t_sensor_update_interval(int grid_power_phase_t_sensor_update_interval);
			void set_off_grid_power_total_sensor_update_interval(int off_grid_power_total_sensor_update_interval);
			void set_off_grid_frequency_sensor_update_interval(int off_grid_frequency_sensor_update_interval);
			void set_off_grid_voltage_phase_r_sensor_update_interval(int off_grid_voltage_phase_r_sensor_update_interval);
			void set_off_grid_current_phase_r_sensor_update_interval(int off_grid_current_phase_r_sensor_update_interval);
			void set_off_grid_power_phase_r_sensor_update_interval(int off_grid_power_phase_r_sensor_update_interval);
			void set_off_grid_voltage_phase_s_sensor_update_interval(int off_grid_voltage_phase_s_sensor_update_interval);
			void set_off_grid_current_phase_s_sensor_update_interval(int off_grid_current_phase_s_sensor_update_interval);
			void set_off_grid_power_phase_s_sensor_update_interval(int off_grid_power_phase_s_sensor_update_interval);
			void set_off_grid_voltage_phase_t_sensor_update_interval(int off_grid_voltage_phase_t_sensor_update_interval);
			void set_off_grid_current_phase_t_sensor_update_interval(int off_grid_current_phase_t_sensor_update_interval);
			void set_off_grid_power_phase_t_sensor_update_interval(int off_grid_power_phase_t_sensor_update_interval);

			void set_desired_grid_power_sensor_default_value(int64_t default_value);
			void set_minimum_battery_power_sensor_default_value(int64_t default_value);
			void set_maximum_battery_power_sensor_default_value(int64_t default_value);
			void set_energy_storage_mode_sensor_default_value(int64_t default_value);
			void set_battery_conf_id_sensor_default_value(int64_t default_value);
			void set_battery_conf_address_sensor_default_value(int64_t default_value);
			void set_battery_conf_protocol_sensor_default_value(int64_t default_value);
			void set_battery_conf_voltage_nominal_sensor_default_value(float default_value);
			void set_battery_conf_voltage_over_sensor_default_value(float default_value);
			void set_battery_conf_voltage_charge_sensor_default_value(float default_value);
			void set_battery_conf_voltage_lack_sensor_default_value(float default_value);
			void set_battery_conf_voltage_discharge_stop_sensor_default_value(float default_value);
			void set_battery_conf_current_charge_limit_sensor_default_value(float default_value);
			void set_battery_conf_current_discharge_limit_sensor_default_value(float default_value);
			void set_battery_conf_depth_of_discharge_sensor_default_value(int64_t default_value);
			void set_battery_conf_end_of_discharge_sensor_default_value(int64_t default_value);
			void set_battery_conf_capacity_sensor_default_value(int64_t default_value);
			void set_battery_conf_cell_type_sensor_default_value(int64_t default_value);
			void set_battery_conf_eps_buffer_sensor_default_value(int64_t default_value);

			void set_desired_grid_power_sensor_enforce_default_value(bool enforce_default_value);
			void set_minimum_battery_power_sensor_enforce_default_value(bool enforce_default_value);
			void set_maximum_battery_power_sensor_enforce_default_value(bool enforce_default_value);
			void set_energy_storage_mode_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_id_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_address_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_protocol_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_voltage_nominal_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_voltage_over_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_voltage_charge_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_voltage_lack_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_voltage_discharge_stop_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_current_charge_limit_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_current_discharge_limit_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_depth_of_discharge_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_end_of_discharge_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_capacity_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_cell_type_sensor_enforce_default_value(bool enforce_default_value);
			void set_battery_conf_eps_buffer_sensor_enforce_default_value(bool enforce_default_value);

            std::string model_;
            int modbus_address_;
            bool zero_export_;
            sensor::Sensor *power_sensor_;

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
			sensor::Sensor *grid_frequency_sensor_{nullptr};
            sensor::Sensor *grid_voltage_phase_r_sensor_{nullptr};
            sensor::Sensor *grid_current_phase_r_sensor_{nullptr};
            sensor::Sensor *grid_power_phase_r_sensor_{nullptr};
            sensor::Sensor *grid_voltage_phase_s_sensor_{nullptr};
            sensor::Sensor *grid_current_phase_s_sensor_{nullptr};
            sensor::Sensor *grid_power_phase_s_sensor_{nullptr};
            sensor::Sensor *grid_voltage_phase_t_sensor_{nullptr};
            sensor::Sensor *grid_current_phase_t_sensor_{nullptr};
            sensor::Sensor *grid_power_phase_t_sensor_{nullptr};
            sensor::Sensor *off_grid_power_total_sensor_{nullptr};
            sensor::Sensor *off_grid_frequency_sensor_{nullptr};
            sensor::Sensor *off_grid_voltage_phase_r_sensor_{nullptr};
            sensor::Sensor *off_grid_current_phase_r_sensor_{nullptr};
            sensor::Sensor *off_grid_power_phase_r_sensor_{nullptr};
            sensor::Sensor *off_grid_voltage_phase_s_sensor_{nullptr};
            sensor::Sensor *off_grid_current_phase_s_sensor_{nullptr};
            sensor::Sensor *off_grid_power_phase_s_sensor_{nullptr};
            sensor::Sensor *off_grid_voltage_phase_t_sensor_{nullptr};
            sensor::Sensor *off_grid_current_phase_t_sensor_{nullptr};
            sensor::Sensor *off_grid_power_phase_t_sensor_{nullptr};

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
			int grid_frequency_sensor_update_interval_;
            int grid_voltage_phase_r_sensor_update_interval_;
            int grid_current_phase_r_sensor_update_interval_;
            int grid_power_phase_r_sensor_update_interval_;
            int grid_voltage_phase_s_sensor_update_interval_;
            int grid_current_phase_s_sensor_update_interval_;
            int grid_power_phase_s_sensor_update_interval_;
            int grid_voltage_phase_t_sensor_update_interval_;
            int grid_current_phase_t_sensor_update_interval_;
            int grid_power_phase_t_sensor_update_interval_;
            int off_grid_power_total_sensor_update_interval_;
            int off_grid_frequency_sensor_update_interval_;
            int off_grid_voltage_phase_r_sensor_update_interval_;
            int off_grid_current_phase_r_sensor_update_interval_;
            int off_grid_power_phase_r_sensor_update_interval_;
            int off_grid_voltage_phase_s_sensor_update_interval_;
            int off_grid_current_phase_s_sensor_update_interval_;
            int off_grid_power_phase_s_sensor_update_interval_;
            int off_grid_voltage_phase_t_sensor_update_interval_;
            int off_grid_current_phase_t_sensor_update_interval_;
            int off_grid_power_phase_t_sensor_update_interval_;
		};
    }
}