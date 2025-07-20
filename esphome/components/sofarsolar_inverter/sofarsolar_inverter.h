#pragma once
#include "queue"
#include "vector"
#include "map"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/modbus/modbus.h"
#include "esphome/components/modbus_controller/modbus_controller.h"
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
#define BATTERY_ACTIVE_CONTROL 60
#define BATTERY_ACTIVE_ONESHOT 61

#define NONE 0
#define SINGLE_REGISTER_WRITE 1
#define DESIRED_GRID_POWER_WRITE 2
#define BATTERY_CONF_WRITE 3
#define BATTERY_ACTIVE_WRITE 4

namespace esphome {
    namespace sofarsolar_inverter {

		union SofarSolar_RegisterValue {
			uint16_t uint16_value;
			int16_t int16_value;
			uint32_t uint32_value;
			int32_t int32_value;
			uint64_t uint64_value;
		    int64_t int64_value;
            float float_value;
            double double_value;
		};

        struct SofarSolar_Register;

		struct register_read_task;

        struct register_write_task;

		static const std::map<uint8_t, SofarSolar_Register> G3_registers = {
			{PV_GENERATION_TODAY, SofarSolar_Register{0x0684, 2, SensorValueType::U_DWORD, 1, -2, NONE}}, // PV Generation Today
            {PV_GENERATION_TOTAL, SofarSolar_Register{0x0686, 2, SensorValueType::U_DWORD, 0, -1, NONE}}, // PV Generation Total
            {LOAD_CONSUMPTION_TODAY, SofarSolar_Register{0x0688, 2, SensorValueType::U_DWORD, 1, -2, NONE}}, // Load Consumption Today
    		{LOAD_CONSUMPTION_TOTAL, SofarSolar_Register{0x068A, 2, SensorValueType::U_DWORD, 0, -1, NONE}}, // Load Consumption Total
            {BATTERY_CHARGE_TODAY, SofarSolar_Register{0x0694, 2, SensorValueType::U_DWORD, 1, -2, NONE}}, // Battery Charge Today
            {BATTERY_CHARGE_TOTAL, SofarSolar_Register{0x0696, 2, SensorValueType::U_DWORD, 0, -1, NONE}}, // Battery Charge Total
            {BATTERY_DISCHARGE_TODAY, SofarSolar_Register{0x0698, 2, SensorValueType::U_DWORD, 1, -2, NONE}}, // Battery Discharge Today
            {BATTERY_DISCHARGE_TOTAL, SofarSolar_Register{0x069A, 2, SensorValueType::U_DWORD, 0, -1, NONE}}, // Battery Discharge Total
            {TOTAL_ACTIVE_POWER_INVERTER,SofarSolar_Register{0x0485, 1, SensorValueType::S_WORD, 3, 1, NONE}}, // Total Active Power Inverter
            {PV_VOLTAGE_1 ,SofarSolar_Register{0x0584, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // PV Voltage 1
            {PV_CURRENT_1 ,SofarSolar_Register{0x0585, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // PV Current 1
            {PV_POWER_1 ,SofarSolar_Register{0x0586, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // PV Power 1
            {PV_VOLTAGE_2 ,SofarSolar_Register{0x0587, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // PV Voltage 2
            {PV_CURRENT_2 ,SofarSolar_Register{0x0588, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // PV Current 2
            {PV_POWER_2, SofarSolar_Register{0x0589, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // PV Power 2
            {PV_POWER_TOTAL, SofarSolar_Register{0x05C4, 1, SensorValueType::U_WORD, 3, 2, NONE}}, // PV Power Total
            {BATTERY_POWER_TOTAL, SofarSolar_Register{0x0667, 1, SensorValueType::S_WORD, 3, 2, NONE}}, // Battery Power Total
            {BATTERY_STATE_OF_CHARGE_TOTAL, SofarSolar_Register{0x0668, 1, SensorValueType::U_WORD, 1, 0, NONE}}, // Battery State of Charge Total
            {DESIRED_GRID_POWER, SofarSolar_Register{0x1187, 2, SensorValueType::S_DWORD, 3, 0, DESIRED_GRID_POWER_WRITE}}, // Desired Grid Power
			{MINIMUM_BATTERY_POWER, SofarSolar_Register{0x1189, 2, SensorValueType::S_DWORD, 3, 0, DESIRED_GRID_POWER_WRITE}}, // Minimum Battery Power
			{MAXIMUM_BATTERY_POWER, SofarSolar_Register{0x118B, 2, SensorValueType::S_DWORD, 3, 0, DESIRED_GRID_POWER_WRITE}}, // Maximum Battery Power
			{ENERGY_STORAGE_MODE, SofarSolar_Register{0x1110, 1, SensorValueType::U_WORD, 0, 0, SINGLE_REGISTER_WRITE}}, // Energy Storage Mode
			{BATTERY_CONF_ID, SofarSolar_Register{0x1044, 1, SensorValueType::U_WORD, 0, 0, BATTERY_CONF_WRITE}}, // Battery Conf ID
			{BATTERY_CONF_ADDRESS, SofarSolar_Register{0x1045, 1, SensorValueType::U_WORD, 0, 0, BATTERY_CONF_WRITE}}, // Battery Conf Address
			{BATTERY_CONF_PROTOCOL, SofarSolar_Register{0x1046, 1, SensorValueType::U_WORD, 0, 0, BATTERY_CONF_WRITE}}, // Battery Conf Protocol
			{BATTERY_CONF_VOLTAGE_NOMINAL, SofarSolar_Register{0x1050, 1, SensorValueType::U_WORD, 0, -1, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Nominal
			{BATTERY_CONF_VOLTAGE_OVER, SofarSolar_Register{0x1047, 1, SensorValueType::U_WORD, 0, -1, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Over
			{BATTERY_CONF_VOLTAGE_CHARGE, SofarSolar_Register{0x1048, 1, SensorValueType::U_WORD, 0, -1, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Charge
			{BATTERY_CONF_VOLTAGE_LACK,SofarSolar_Register{0x1049, 1, SensorValueType::U_WORD, 0, -1, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Lack
			{BATTERY_CONF_VOLTAGE_DISCHARGE_STOP,SofarSolar_Register{0x104A, 1, SensorValueType::U_WORD, 0, -1, BATTERY_CONF_WRITE}}, // Battery Conf Voltage Discharge Stop
			{BATTERY_CONF_CURRENT_CHARGE_LIMIT, SofarSolar_Register{0x104B, 1, SensorValueType::U_WORD, 0, -2, BATTERY_CONF_WRITE}}, // Battery Conf Current Charge Limit
			{BATTERY_CONF_CURRENT_DISCHARGE_LIMIT, SofarSolar_Register{0x104C, 1, SensorValueType::U_WORD, 0, -2, BATTERY_CONF_WRITE}}, // Battery Conf Current Discharge Limit
			{BATTERY_CONF_DEPTH_OF_DISCHARGE, SofarSolar_Register{0x104D, 1, SensorValueType::U_WORD, 0, 0, BATTERY_CONF_WRITE}}, // Battery Conf Depth of Discharge
			{BATTERY_CONF_END_OF_DISCHARGE,SofarSolar_Register{0x104E, 1, SensorValueType::U_WORD, 0, 0, BATTERY_CONF_WRITE}}, // Battery Conf End of Discharge
			{BATTERY_CONF_CAPACITY,SofarSolar_Register{0x104F, 1, SensorValueType::U_WORD, 0, 1, BATTERY_CONF_WRITE}}, // Battery Conf Capacity
			{BATTERY_CONF_CELL_TYPE, SofarSolar_Register{0x1051, 1, SensorValueType::U_WORD, 0, 0, BATTERY_CONF_WRITE}}, // Battery Conf Cell Type
			{BATTERY_CONF_EPS_BUFFER, SofarSolar_Register{0x1052, 1, SensorValueType::U_WORD, 0, 1, BATTERY_CONF_WRITE}}, // Battery Conf EPS Buffer
			{BATTERY_CONF_CONTROL,SofarSolar_Register{0x1053, 1 , SensorValueType::U_WORD, 0, 0, BATTERY_CONF_WRITE}}, // Battery Conf Control
			{GRID_FREQUENCY,SofarSolar_Register{0x0484, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // Grid Frequency
			{GRID_VOLTAGE_PHASE_R,SofarSolar_Register{0x0580, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // Grid Voltage Phase R
			{GRID_CURRENT_PHASE_R,SofarSolar_Register{0x0581, 1 ,SensorValueType::U_WORD, 2, -2, NONE}}, // Grid Current Phase R
			{GRID_POWER_PHASE_R,SofarSolar_Register{0x0582, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // Grid Power Phase R
			{GRID_VOLTAGE_PHASE_S,SofarSolar_Register{0x058C, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // Grid Voltage Phase S
			{GRID_CURRENT_PHASE_S,SofarSolar_Register{0x058D, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // Grid Current Phase S
			{GRID_POWER_PHASE_S,SofarSolar_Register{0x058E, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // Grid Power Phase S
			{GRID_VOLTAGE_PHASE_T,SofarSolar_Register{0x0598, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // Grid Voltage Phase T
			{GRID_CURRENT_PHASE_T,SofarSolar_Register{0x0599, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // Grid Current Phase T
			{GRID_POWER_PHASE_T,SofarSolar_Register{0x059A, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // Grid Power Phase T
			{OFF_GRID_POWER_TOTAL,SofarSolar_Register{0x05A4, 1, SensorValueType::U_WORD, 3, 2, NONE}}, // Off Grid Power Total
			{OFF_GRID_FREQUENCY,SofarSolar_Register{0x05A5, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // Off Grid Frequency
			{OFF_GRID_VOLTAGE_PHASE_R,SofarSolar_Register{0x05A6, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // Off Grid Voltage Phase R
			{OFF_GRID_CURRENT_PHASE_R,SofarSolar_Register{0x05A7, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // Off Grid Current Phase R
			{OFF_GRID_POWER_PHASE_R,SofarSolar_Register{0x05A8, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // Off Grid Power Phase R
			{OFF_GRID_VOLTAGE_PHASE_S,SofarSolar_Register{0x05AC, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // Off Grid Voltage Phase S
			{OFF_GRID_CURRENT_PHASE_S,SofarSolar_Register{0x05AD, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // Off Grid Current Phase S
			{OFF_GRID_POWER_PHASE_S,SofarSolar_Register{0x05AE, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // Off Grid Power Phase S
			{OFF_GRID_VOLTAGE_PHASE_T,SofarSolar_Register{0x05B8, 1, SensorValueType::U_WORD, 2, -1, NONE}}, // Off Grid Voltage Phase T
			{OFF_GRID_CURRENT_PHASE_T,SofarSolar_Register{0x05B9, 1, SensorValueType::U_WORD, 2, -2, NONE}}, // Off Grid Current Phase T
			{OFF_GRID_POWER_PHASE_T,SofarSolar_Register{0x05BA, 1, SensorValueType::U_WORD, 2, 1, NONE}}, // Off Grid Power Phase T
			{BATTERY_ACTIVE_CONTROL, SofarSolar_Register{0x102B, 1, SensorValueType::U_WORD, 0, 0, BATTERY_ACTIVE_WRITE}}, // Battery Active Control
			{BATTERY_ACTIVE_ONESHOT, SofarSolar_Register{0x102C, 1, SensorValueType::U_WORD, 0, 0, BATTERY_ACTIVE_WRITE}} // Battery Active Oneshot
        };

        class SofarSolar_Inverter : public modbus::ModbusDevice, public Component {
        public:

            std::map<uint8_t, SofarSolar_RegisterTime> G3_dynamic;

            SofarSolar_Inverter();

            void setup() override;
            void loop() override;
            void dump_config() override;

			void on_modbus_data(const std::vector<uint8_t> &data) override;
			void on_modbus_error(uint8_t function_code, uint8_t exception_code) override;

			void parse_read_response(const std::vector<uint8_t> &data);
			void parse_write_response(const std::vector<uint8_t> &data);

            void update_sensor(const register_read_task task);
            void send_modbus(std::vector<uint8_t> frame);
            void read_modbus_registers(uint16_t start_address, uint16_t quantity);
            bool receive_modbus_response(std::vector<uint8_t> &response, uint8_t response_length);
            void write_modbus_registers(uint16_t start_address, uint16_t quantity, const std::vector<uint8_t> &data);

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
            bool extract_data_from_response(std::vector<uint8_t> &response, register_read_task &task);
            bool write_response(register_write_task &task);

			void write_desired_grid_power(register_write_task &task);
			void write_battery_conf(register_write_task &task);
			void write_battery_active(register_write_task &task);
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
			void set_battery_active_control_sensor(sensor::Sensor *battery_active_control_sensor);
			void set_battery_active_oneshot_sensor(sensor::Sensor *battery_active_oneshot_sensor);

            // Set update intervals for sensors
            void set_pv_generation_today_sensor_update_interval(uint16_t pv_generation_today_sensor_update_interval);
            void set_pv_generation_total_sensor_update_interval(uint16_t pv_generation_total_sensor_update_interval);
            void set_load_consumption_today_sensor_update_interval(uint16_t load_consumption_today_sensor_update_interval);
            void set_load_consumption_total_sensor_update_interval(uint16_t load_consumption_total_sensor_update_interval);
            void set_battery_charge_today_sensor_update_interval(uint16_t battery_charge_today_sensor_update_interval);
            void set_battery_charge_total_sensor_update_interval(uint16_t battery_charge_total_sensor_update_interval);
            void set_battery_discharge_today_sensor_update_interval(uint16_t battery_discharge_today_sensor_update_interval);
            void set_battery_discharge_total_sensor_update_interval(uint16_t battery_discharge_total_sensor_update_interval);
            void set_total_active_power_inverter_sensor_update_interval(uint16_t total_active_power_inverter_sensor_update_interval);
            void set_pv_voltage_1_sensor_update_interval(uint16_t pv_voltage_1_sensor_update_interval);
            void set_pv_current_1_sensor_update_interval(uint16_t pv_current_1_sensor_update_interval);
            void set_pv_power_1_sensor_update_interval(uint16_t pv_power_1_sensor_update_interval);
            void set_pv_voltage_2_sensor_update_interval(uint16_t pv_voltage_2_sensor_update_interval);
            void set_pv_current_2_sensor_update_interval(uint16_t pv_current_2_sensor_update_interval);
            void set_pv_power_2_sensor_update_interval(uint16_t pv_power_2_sensor_update_interval);
            void set_pv_power_total_sensor_update_interval(uint16_t pv_power_total_sensor_update_interval);
            void set_battery_power_total_sensor_update_interval(uint16_t battery_power_total_sensor_update_interval);
            void set_battery_state_of_charge_total_sensor_update_interval(uint16_t battery_state_of_charge_total_sensor_update_interval);
            void set_desired_grid_power_sensor_update_interval(uint16_t desired_grid_power_sensor_update_interval);
            void set_minimum_battery_power_sensor_update_interval(uint16_t minimum_battery_power_sensor_update_interval);
            void set_maximum_battery_power_sensor_update_interval(uint16_t maximum_battery_power_sensor_update_interval);
            void set_energy_storage_mode_sensor_update_interval(uint16_t energy_storage_mode_sensor_update_interval);
            void set_battery_conf_id_sensor_update_interval(uint16_t battery_conf_id_sensor_update_interval);
            void set_battery_conf_address_sensor_update_interval(uint16_t battery_conf_address_sensor_update_interval);
            void set_battery_conf_protocol_sensor_update_interval(uint16_t battery_conf_protocol_sensor_update_interval);
            void set_battery_conf_voltage_nominal_sensor_update_interval(uint16_t battery_conf_voltage_nominal_sensor_update_interval);
            void set_battery_conf_voltage_over_sensor_update_interval(uint16_t battery_conf_voltage_over_sensor_update_interval);
            void set_battery_conf_voltage_charge_sensor_update_interval(uint16_t battery_conf_voltage_charge_sensor_update_interval);
            void set_battery_conf_voltage_lack_sensor_update_interval(uint16_t battery_conf_voltage_lack_sensor_update_interval);
            void set_battery_conf_voltage_discharge_stop_sensor_update_interval(uint16_t battery_conf_voltage_discharge_stop_sensor_update_interval);
            void set_battery_conf_current_charge_limit_sensor_update_interval(uint16_t battery_conf_current_charge_limit_sensor_update_interval);
            void set_battery_conf_current_discharge_limit_sensor_update_interval(uint16_t battery_conf_current_discharge_limit_sensor_update_interval);
            void set_battery_conf_depth_of_discharge_sensor_update_interval(uint16_t battery_conf_depth_of_discharge_sensor_update_interval);
            void set_battery_conf_end_of_discharge_sensor_update_interval(uint16_t battery_conf_end_of_discharge_sensor_update_interval);
            void set_battery_conf_capacity_sensor_update_interval(uint16_t battery_conf_capacity_sensor_update_interval);
            void set_battery_conf_cell_type_sensor_update_interval(uint16_t battery_conf_cell_type_sensor_update_interval);
            void set_battery_conf_eps_buffer_sensor_update_interval(uint16_t battery_conf_eps_buffer_sensor_update_interval);
            void set_battery_conf_control_sensor_update_interval(uint16_t battery_conf_control_sensor_update_interval);
			void set_grid_frequency_sensor_update_interval(uint16_t grid_frequency_sensor_update_interval);
			void set_grid_voltage_phase_r_sensor_update_interval(uint16_t grid_voltage_phase_r_sensor_update_interval);
			void set_grid_current_phase_r_sensor_update_interval(uint16_t grid_current_phase_r_sensor_update_interval);
			void set_grid_power_phase_r_sensor_update_interval(uint16_t grid_power_phase_r_sensor_update_interval);
			void set_grid_voltage_phase_s_sensor_update_interval(uint16_t grid_voltage_phase_s_sensor_update_interval);
			void set_grid_current_phase_s_sensor_update_interval(uint16_t grid_current_phase_s_sensor_update_interval);
			void set_grid_power_phase_s_sensor_update_interval(uint16_t grid_power_phase_s_sensor_update_interval);
			void set_grid_voltage_phase_t_sensor_update_interval(uint16_t grid_voltage_phase_t_sensor_update_interval);
			void set_grid_current_phase_t_sensor_update_interval(uint16_t grid_current_phase_t_sensor_update_interval);
			void set_grid_power_phase_t_sensor_update_interval(uint16_t grid_power_phase_t_sensor_update_interval);
			void set_off_grid_power_total_sensor_update_interval(uint16_t off_grid_power_total_sensor_update_interval);
			void set_off_grid_frequency_sensor_update_interval(uint16_t off_grid_frequency_sensor_update_interval);
			void set_off_grid_voltage_phase_r_sensor_update_interval(uint16_t off_grid_voltage_phase_r_sensor_update_interval);
			void set_off_grid_current_phase_r_sensor_update_interval(uint16_t off_grid_current_phase_r_sensor_update_interval);
			void set_off_grid_power_phase_r_sensor_update_interval(uint16_t off_grid_power_phase_r_sensor_update_interval);
			void set_off_grid_voltage_phase_s_sensor_update_interval(uint16_t off_grid_voltage_phase_s_sensor_update_interval);
			void set_off_grid_current_phase_s_sensor_update_interval(uint16_t off_grid_current_phase_s_sensor_update_interval);
			void set_off_grid_power_phase_s_sensor_update_interval(uint16_t off_grid_power_phase_s_sensor_update_interval);
			void set_off_grid_voltage_phase_t_sensor_update_interval(uint16_t off_grid_voltage_phase_t_sensor_update_interval);
			void set_off_grid_current_phase_t_sensor_update_interval(uint16_t off_grid_current_phase_t_sensor_update_interval);
			void set_off_grid_power_phase_t_sensor_update_interval(uint16_t off_grid_power_phase_t_sensor_update_interval);
			void set_battery_active_control_sensor_update_interval(uint16_t battery_active_control_sensor_update_interval);
			void set_battery_active_oneshot_sensor_update_interval(uint16_t battery_active_oneshot_sensor_update_interval);

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
			sensor::Sensor *battery_active_control_sensor_{nullptr};
			sensor::Sensor *battery_active_oneshot_sensor_{nullptr};

            uint16_t pv_generation_today_sensor_update_interval_;
            uint16_t pv_generation_total_sensor_update_interval_;
            uint16_t load_consumption_today_sensor_update_interval_;
            uint16_t load_consumption_total_sensor_update_interval_;
            uint16_t battery_charge_today_sensor_update_interval_;
            uint16_t battery_charge_total_sensor_update_interval_;
            uint16_t battery_discharge_today_sensor_update_interval_;
            uint16_t battery_discharge_total_sensor_update_interval_;
            uint16_t total_active_power_inverter_sensor_update_interval_;
            uint16_t pv_voltage_1_sensor_update_interval_;
            uint16_t pv_current_1_sensor_update_interval_;
            uint16_t pv_power_1_sensor_update_interval_;
            uint16_t pv_voltage_2_sensor_update_interval_;
            uint16_t pv_current_2_sensor_update_interval_;
            uint16_t pv_power_2_sensor_update_interval_;
            uint16_t pv_power_total_sensor_update_interval_;
            uint16_t battery_power_total_sensor_update_interval_;
            uint16_t battery_state_of_charge_total_sensor_update_interval_;
            uint16_t desired_grid_power_sensor_update_interval_;
            uint16_t minimum_battery_power_sensor_update_interval_;
            uint16_t maximum_battery_power_sensor_update_interval_;
            uint16_t energy_storage_mode_sensor_update_interval_;
            uint16_t battery_conf_id_sensor_update_interval_;
            uint16_t battery_conf_address_sensor_update_interval_;
            uint16_t battery_conf_protocol_sensor_update_interval_;
            uint16_t battery_conf_voltage_nominal_sensor_update_interval_;
            uint16_t battery_conf_voltage_over_sensor_update_interval_;
            uint16_t battery_conf_voltage_charge_sensor_update_interval_;
            uint16_t battery_conf_voltage_lack_sensor_update_interval_;
            uint16_t battery_conf_voltage_discharge_stop_sensor_update_interval_;
            uint16_t battery_conf_current_charge_limit_sensor_update_interval_;
            uint16_t battery_conf_current_discharge_limit_sensor_update_interval_;
            uint16_t battery_conf_depth_of_discharge_sensor_update_interval_;
            uint16_t battery_conf_end_of_discharge_sensor_update_interval_;
            uint16_t battery_conf_capacity_sensor_update_interval_;
            uint16_t battery_conf_cell_type_sensor_update_interval_;
            uint16_t battery_conf_eps_buffer_sensor_update_interval_;
            uint16_t battery_conf_control_sensor_update_interval_;
			uint16_t grid_frequency_sensor_update_interval_;
            uint16_t grid_voltage_phase_r_sensor_update_interval_;
            uint16_t grid_current_phase_r_sensor_update_interval_;
            uint16_t grid_power_phase_r_sensor_update_interval_;
            uint16_t grid_voltage_phase_s_sensor_update_interval_;
            uint16_t grid_current_phase_s_sensor_update_interval_;
            uint16_t grid_power_phase_s_sensor_update_interval_;
            uint16_t grid_voltage_phase_t_sensor_update_interval_;
            uint16_t grid_current_phase_t_sensor_update_interval_;
            uint16_t grid_power_phase_t_sensor_update_interval_;
            uint16_t off_grid_power_total_sensor_update_interval_;
            uint16_t off_grid_frequency_sensor_update_interval_;
            uint16_t off_grid_voltage_phase_r_sensor_update_interval_;
            uint16_t off_grid_current_phase_r_sensor_update_interval_;
            uint16_t off_grid_power_phase_r_sensor_update_interval_;
            uint16_t off_grid_voltage_phase_s_sensor_update_interval_;
            uint16_t off_grid_current_phase_s_sensor_update_interval_;
            uint16_t off_grid_power_phase_s_sensor_update_interval_;
            uint16_t off_grid_voltage_phase_t_sensor_update_interval_;
            uint16_t off_grid_current_phase_t_sensor_update_interval_;
            uint16_t off_grid_power_phase_t_sensor_update_interval_;
			uint16_t battery_active_control_sensor_update_interval_;
			uint16_t battery_active_oneshot_sensor_update_interval_;

			SofarSolar_RegisterValue desired_grid_power_sensor_default_value_ = {};
			SofarSolar_RegisterValue minimum_battery_power_sensor_default_value_ = {};
			SofarSolar_RegisterValue maximum_battery_power_sensor_default_value_ = {};
			SofarSolar_RegisterValue energy_storage_mode_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_id_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_address_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_protocol_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_voltage_nominal_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_voltage_over_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_voltage_charge_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_voltage_lack_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_voltage_discharge_stop_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_current_charge_limit_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_current_discharge_limit_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_depth_of_discharge_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_end_of_discharge_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_capacity_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_cell_type_sensor_default_value_ = {};
			SofarSolar_RegisterValue battery_conf_eps_buffer_sensor_default_value_ = {};

			bool desired_grid_power_sensor_default_value_set_{false};
			bool minimum_battery_power_sensor_default_value_set_{false};
            bool maximum_battery_power_sensor_default_value_set_{false};
            bool energy_storage_mode_sensor_default_value_set_{false};
            bool battery_conf_id_sensor_default_value_set_{false};
            bool battery_conf_address_sensor_default_value_set_{false};
            bool battery_conf_protocol_sensor_default_value_set_{false};
            bool battery_conf_voltage_nominal_sensor_default_value_set_{false};
            bool battery_conf_voltage_over_sensor_default_value_set_{false};
            bool battery_conf_voltage_charge_sensor_default_value_set_{false};
            bool battery_conf_voltage_lack_sensor_default_value_set_{false};
            bool battery_conf_voltage_discharge_stop_sensor_default_value_set_{false};
            bool battery_conf_current_charge_limit_sensor_default_value_set_{false};
            bool battery_conf_current_discharge_limit_sensor_default_value_set_{false};
            bool battery_conf_depth_of_discharge_sensor_default_value_set_{false};
            bool battery_conf_end_of_discharge_sensor_default_value_set_{false};
            bool battery_conf_capacity_sensor_default_value_set_{false};
            bool battery_conf_cell_type_sensor_default_value_set_{false};
            bool battery_conf_eps_buffer_sensor_default_value_set_{false};

            bool desired_grid_power_sensor_enforce_default_value_{false};
            bool minimum_battery_power_sensor_enforce_default_value_{false};
            bool maximum_battery_power_sensor_enforce_default_value_{false};
            bool energy_storage_mode_sensor_enforce_default_value_{false};
            bool battery_conf_id_sensor_enforce_default_value_{false};
            bool battery_conf_address_sensor_enforce_default_value_{false};
            bool battery_conf_protocol_sensor_enforce_default_value_{false};
            bool battery_conf_voltage_nominal_sensor_enforce_default_value_{false};
            bool battery_conf_voltage_over_sensor_enforce_default_value_{false};
            bool battery_conf_voltage_charge_sensor_enforce_default_value_{false};
            bool battery_conf_voltage_lack_sensor_enforce_default_value_{false};
            bool battery_conf_voltage_discharge_stop_sensor_enforce_default_value_{false};
            bool battery_conf_current_charge_limit_sensor_enforce_default_value_{false};
            bool battery_conf_current_discharge_limit_sensor_enforce_default_value_{false};
            bool battery_conf_depth_of_discharge_sensor_enforce_default_value_{false};
            bool battery_conf_end_of_discharge_sensor_enforce_default_value_{false};
            bool battery_conf_capacity_sensor_enforce_default_value_{false};
            bool battery_conf_cell_type_sensor_enforce_default_value_{false};
            bool battery_conf_eps_buffer_sensor_enforce_default_value_{false};
		};
    }
}