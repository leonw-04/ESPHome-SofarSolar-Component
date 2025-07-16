#pragma once
#include "queue"
#include "vector"
#include "map"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

#define PV_GENERATION_TODAY = 1;
#define PV_GENERATION_TOTAL = 2;
#define LOAD_CONSUMPTION_TODAY = 3;
#define LOAD_CONSUMPTION_TOTAL = 4;
#define BATTERY_CHARGE_TODAY = 5;
#define BATTERY_CHARGE_TOTAL = 6;
#define BATTERY_DISCHARGE_TODAY = 7;
#define BATTERY_DISCHARGE_TOTAL = 8;
#define TOTAL_ACTIVE_POWER_INVERTER = 9;
#define PV_VOLTAGE_1 = 10;
#define PV_CURRENT_1 = 11;
#define PV_POWER_1 = 12;
#define PV_VOLTAGE_2 = 13;
#define PV_CURRENT_2 = 14;
#define PV_POWER_2 = 15;
#define PV_POWER_TOTAL = 16;
#define BATTERY_POWER_TOTAL = 17;
#define BATTERY_STATE_OF_CHARGE_TOTAL = 18;
#define DESIRED_GRID_POWER = 19;
#define MINIMUM_BATTERY_POWER = 20;
#define MAXIMUM_BATTERY_POWER = 21;
#define ENERGY_STORAGE_MODE = 22;
#define BATTERY_CONF_ID = 23;
#define BATTERY_CONF_ADDRESS = 24;
#define BATTERY_CONF_PROTOCOL = 25;
#define BATTERY_CONF_VOLTAGE_NOMINAL = 26;
#define BATTERY_CONF_VOLTAGE_OVER = 27;
#define BATTERY_CONF_VOLTAGE_CHARGE = 28;
#define BATTERY_CONF_VOLTAGE_LACK = 29;
#define BATTERY_CONF_VOLTAGE_DISCHARGE_STOP = 30;
#define BATTERY_CONF_CURRENT_CHARGE_LIMIT = 31;
#define BATTERY_CONF_CURRENT_DISCHARGE_LIMIT = 32;
#define BATTERY_CONF_DEPTH_OF_DISCHARGE = 33;
#define BATTERY_CONF_END_OF_DISCHARGE = 34;
#define BATTERY_CONF_CAPACITY = 35;
#define BATTERY_CONF_CELL_TYPE = 36;
#define BATTERY_CONF_EPS_BUFFER = 37;
#define BATTERY_CONF_CONTROL = 38;
#define GRID_FREQUENCY = 39;
#define GRID_VOLTAGE_PHASE_R = 40;
#define GRID_CURRENT_PHASE_R = 41;
#define GRID_POWER_PHASE_R = 42;
#define GRID_VOLTAGE_PHASE_S = 43;
#define GRID_CURRENT_PHASE_S = 44;
#define GRID_POWER_PHASE_S = 45;
#define GRID_VOLTAGE_PHASE_T = 46;
#define GRID_CURRENT_PHASE_T = 47;
#define GRID_POWER_PHASE_T = 48;
#define OFF_GRID_POWER_TOTAL = 49;
#define OFF_GRID_FREQUENCY = 50;
#define OFF_GRID_VOLTAGE_PHASE_R = 51;
#define OFF_GRID_CURRENT_PHASE_R = 52;
#define OFF_GRID_POWER_PHASE_R = 53;
#define OFF_GRID_VOLTAGE_PHASE_S = 54;
#define OFF_GRID_CURRENT_PHASE_S = 55;
#define OFF_GRID_POWER_PHASE_S = 56;
#define OFF_GRID_VOLTAGE_PHASE_T = 57;
#define OFF_GRID_CURRENT_PHASE_T = 58;
#define OFF_GRID_POWER_PHASE_T = 59;

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
			void (*write_funktion)() = nullptr; // Function pointer for writing to the register
            SofarSolar_Register(uint16_t start_address, uint16_t quantity, uint8_t type, uint8_t priority, float scale, bool writeable, void (*write_funktion)() = nullptr) : start_address(start_address), quantity(quantity), type(type), priority(priority), scale(scale), writeable(writeable), write_funktion(write_funktion) {}
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


        class SofarSolar_Inverter : public uart::UARTDevice, public Component {
        public:

            std::map<uint8_t, SofarSolar_Register> registers_G3;
			registers_G3[PV_GENERATION_TODAY] = SofarSolar_Register{0x0684, 2, 2, 1, 0.01, false}; // PV Generation Today
			registers_G3[PV_GENERATION_TOTAL] = SofarSolar_Register{0x0686, 2, 2, 0, 0.1, false}; // PV Generation Total
			registers_G3[LOAD_CONSUMPTION_TODAY] = SofarSolar_Register{0x0688, 2, 2, 1, 0.01, false}; // Load Consumption Today
			registers_G3[LOAD_CONSUMPTION_TOTAL] = SofarSolar_Register{0x068A, 2, 2, 0, 0.1, false}; // Load Consumption Total
			registers_G3[BATTERY_CHARGE_TODAY] = SofarSolar_Register{0x0694, 2, 2, 1, 0.01, false}; // Battery Charge Today
			registers_G3[BATTERY_CHARGE_TOTAL] = SofarSolar_Register{0x0696, 2, 2, 0, 0.1, false}; // Battery Charge Total
			registers_G3[BATTERY_DISCHARGE_TODAY] = SofarSolar_Register{0x0698, 2, 2, 1, 0.01, false}; // Battery Discharge Today
			registers_G3[BATTERY_DISCHARGE_TOTAL] = SofarSolar_Register{0x069A, 2, 2, 0, 0.1, false}; // Battery Discharge Total
			registers_G3[TOTAL_ACTIVE_POWER_INVERTER] = SofarSolar_Register{0x0485, 1, 1, 3, 10, false}; // Total Active Power Inverter
            registers_G3[PV_VOLTAGE_1] = SofarSolar_Register{0x0584, 1, 0, 2, 0.1, false}; // PV Voltage 1
            registers_G3[PV_CURRENT_1] = SofarSolar_Register{0x0585, 1, 0, 2, 0.01, false}; // PV Current 1
            registers_G3[PV_POWER_1] = SofarSolar_Register{0x0586, 1, 0, 2, 10, false}; // PV Power 1
            registers_G3[PV_VOLTAGE_2] = SofarSolar_Register{0x0588, 1, 0, 2, 0.1, false}; // PV Voltage 2
            registers_G3[PV_CURRENT_2] = SofarSolar_Register{0x0589, 1, 0, 2, 0.01, false}; // PV Current 2
            registers_G3[PV_POWER_2] = SofarSolar_Register{0x058A, 1, 0, 2, 10, false}; // PV Power 2
            registers_G3[PV_POWER_TOTAL] = SofarSolar_Register{0x05C4, 1, 0, 3, 100, false}; // PV Power Total
            registers_G3[BATTERY_POWER_TOTAL] = SofarSolar_Register{0x0667, 1, 1, 3, 100, false}; // Battery Power Total
            registers_G3[BATTERY_STATE_OF_CHARGE_TOTAL] = SofarSolar_Register{0x0668, 1, 0, 1, 1, false}; // Battery State of Charge Total
            registers_G3[DESIRED_GRID_POWER] = SofarSolar_Register{0x1187, 2, 3, 3, 1, true, write_desired_grid_power}; // Desired Grid Power
            registers_G3[MINIMUM_BATTERY_POWER] = SofarSolar_Register{0x1189, 2, 3, 3, 1, true, write_desired_grid_power}; // Minimum Battery Power
            registers_G3[MAXIMUM_BATTERY_POWER] = SofarSolar_Register{0x118B, 2, 3, 3, 1, true, write_desired_grid_power}; // Maximum Battery Power
            registers_G3[ENERGY_STORAGE_MODE] = SofarSolar_Register{0x1110, 1, 0, 0, 1 ,true, write_single_register}; // Energy Storage Mode
            registers_G3[BATTERY_CONF_ID] = SofarSolar_Register{0x1044, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf ID
            registers_G3[BATTERY_CONF_ADDRESS] = SofarSolar_Register{0x1045, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf Address
            registers_G3[BATTERY_CONF_PROTOCOL] = SofarSolar_Register{0x1046, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf Protocol
            registers_G3[BATTERY_CONF_VOLTAGE_NOMINAL] = SofarSolar_Register{0x1050, 1, 0, 0, 0.1, true, write_battery_conf}; // Battery Conf Voltage Nominal
            registers_G3[BATTERY_CONF_VOLTAGE_OVER] = SofarSolar_Register{0x1047, 1, 0, 0, 0.1, true, write_battery_conf}; // Battery Conf Voltage Over
            registers_G3[BATTERY_CONF_VOLTAGE_CHARGE] = SofarSolar_Register{0x1048, 1, 0, 0, 0.1, true, write_battery_conf}; // Battery Conf Voltage Charge
            registers_G3[BATTERY_CONF_VOLTAGE_LACK] = SofarSolar_Register{0x1049, 1, 0, 0, 0.1, true, write_battery_conf}; // Battery Conf Voltage Lack
            registers_G3[BATTERY_CONF_VOLTAGE_DISCHARGE_STOP] = SofarSolar_Register{0x104A, 1, 0, 0, 0.1, true, write_battery_conf}; // Battery Conf Voltage Discharge Stop
            registers_G3[BATTERY_CONF_CURRENT_CHARGE_LIMIT] = SofarSolar_Register{0x104B, 1, 0, 0, 0.01, true, write_battery_conf}; // Battery Conf Current Charge Limit
            registers_G3[BATTERY_CONF_CURRENT_DISCHARGE_LIMIT] = SofarSolar_Register{0x104C, 1, 0, 0, 0.01, true, write_battery_conf}; // Battery Conf Current Discharge Limit
            registers_G3[BATTERY_CONF_DEPTH_OF_DISCHARGE] = SofarSolar_Register{0x104D, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf Depth of Discharge
            registers_G3[BATTERY_CONF_END_OF_DISCHARGE] = SofarSolar_Register{0x104E, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf End of Discharge
            registers_G3[BATTERY_CONF_CAPACITY] = SofarSolar_Register{0x104F, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf Capacity
            registers_G3[BATTERY_CONF_CELL_TYPE] = SofarSolar_Register{0x1051, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf Cell Type
            registers_G3[BATTERY_CONF_EPS_BUFFER] = SofarSolar_Register{0x1052, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf EPS Buffer
            registers_G3[BATTERY_CONF_CONTROL] = SofarSolar_Register{0x1053, 1, 0, 0, 1, true, write_battery_conf}; // Battery Conf Control
            registers_G3[GRID_FREQUENCY] = SofarSolar_Register{0x0484, 1, 0, 1, 0.01, false}; // Grid Frequency
            registers_G3[GRID_VOLTAGE_PHASE_R] = SofarSolar_Register{0x048D, 1, 0, 1, 0.1, false}; // Grid Voltage Phase R
            registers_G3[GRID_CURRENT_PHASE_R] = SofarSolar_Register{0x048E, 1, 1, 1, 0.01, false}; // Grid Current Phase R
            registers_G3[GRID_POWER_PHASE_R] = SofarSolar_Register{0x048F, 1, 1, 1, 10, false}; // Grid Power Phase R
            registers_G3[GRID_VOLTAGE_PHASE_S] = SofarSolar_Register{0x0498, 1, 0, 1, 0.1, false}; // Grid Voltage Phase S
			registers_G3[GRID_CURRENT_PHASE_S] = SofarSolar_Register{0x0499, 1, 1, 1, 0.01, false}; // Grid Current Phase S
            registers_G3[GRID_POWER_PHASE_S] = SofarSolar_Register{0x049A, 1, 1, 1, 10, false}; // Grid Power Phase S
            registers_G3[GRID_VOLTAGE_PHASE_T] = SofarSolar_Register{0x04A3, 1, 0, 1, 0.1, false}; // Grid Voltage Phase T
            registers_G3[GRID_CURRENT_PHASE_T] = SofarSolar_Register{0x04A4, 1, 1, 1, 0.01, false}; // Grid Current Phase T
            registers_G3[GRID_POWER_PHASE_T] = SofarSolar_Register{0x04A5, 1, 1, 1, 10, false}; // Grid Power Phase T
            registers_G3[OFF_GRID_POWER_TOTAL] = SofarSolar_Register{0x0504, 1, 1, 1, 10, false}; // Off Grid Power Total
            registers_G3[OFF_GRID_FREQUENCY] = SofarSolar_Register{0x0507, 1, 0, 1, 0.01, false}; // Off Grid Frequency
            registers_G3[OFF_GRID_VOLTAGE_PHASE_R] = SofarSolar_Register{0x050A, 1, 0, 1, 0.1, false}; // Off Grid Voltage Phase R
            registers_G3[OFF_GRID_CURRENT_PHASE_R] = SofarSolar_Register{0x050B, 1, 1, 1, 0.01, false}; // Off Grid Current Phase R
            registers_G3[OFF_GRID_POWER_PHASE_R] = SofarSolar_Register{0x050C, 1, 1, 1, 10, false}; // Off Grid Power Phase R
            registers_G3[OFF_GRID_VOLTAGE_PHASE_S] = SofarSolar_Register{0x0512, 1, 0, 1, 0.1, false}; // Off Grid Voltage Phase S
            registers_G3[OFF_GRID_CURRENT_PHASE_S] = SofarSolar_Register{0x0513, 1, 1, 1, 0.01, false}; // Off Grid Current
            registers_G3[OFF_GRID_POWER_PHASE_S] = SofarSolar_Register{0x0514, 1, 1, 1, 10, false}; // Off Grid Power Phase S
            registers_G3[OFF_GRID_VOLTAGE_PHASE_T] = SofarSolar_Register{0x051A, 1, 0, 1, 0.1, false}; // Off Grid Voltage Phase T
            registers_G3[OFF_GRID_CURRENT_PHASE_T] = SofarSolar_Register{0x051B, 1, 1, 1, 0.01, false}; // Off Grid Current Phase T
            registers_G3[OFF_GRID_POWER_PHASE_T] = SofarSolar_Register{0x051C, 1, 1, 1, 10, false}; // Off Grid Power Phase T


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
			uint16_t uint16_t_from_bytes(const std::vector<uint8_t> &data, size_t offset) {
				return (uint16_t) (data[offset] << 8) | data[offset + 1];
			}
			int16_t int16_t_from_bytes(const std::vector<uint8_t> &data, size_t offset) {
				return (int16_t) (data[offset] << 8) | data[offset + 1];
			}
			uint32_t uint32_t_from_bytes(const std::vector<uint8_t> &data, size_t offset) {
				return (uint32_t) (data[offset] << 24) | (data[offset + 1] << 16) | (data[offset + 2] << 8) | data[offset + 3];
			}
			int32_t int32_t_from_bytes(const std::vector<uint8_t> &data, size_t offset) {
				return (int32_t) (data[offset] << 24) | (data[offset + 1] << 16) | (data[offset + 2] << 8) | data[offset + 3];
			}

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

            void set_pv_generation_today_sensor(sensor::Sensor *pv_generation_today_sensor) { registers_G3[0].sensor = pv_generation_today_sensor; pv_generation_today_sensor_ = pv_generation_today_sensor; }
            void set_pv_generation_total_sensor(sensor::Sensor *pv_generation_total_sensor) { registers_G3[1].sensor = pv_generation_total_sensor; pv_generation_total_sensor_ = pv_generation_total_sensor; }
            void set_load_consumption_today_sensor(sensor::Sensor *load_consumption_today_sensor) { registers_G3[2].sensor = load_consumption_today_sensor; load_consumption_today_sensor_ = load_consumption_today_sensor; }
            void set_load_consumption_total_sensor(sensor::Sensor *load_consumption_total_sensor) { registers_G3[3].sensor = load_consumption_total_sensor; load_consumption_total_sensor_ = load_consumption_total_sensor; }
            void set_battery_charge_today_sensor(sensor::Sensor *battery_charge_today_sensor) { registers_G3[4].sensor = battery_charge_today_sensor; battery_charge_today_sensor_ = battery_charge_today_sensor; }
            void set_battery_charge_total_sensor(sensor::Sensor *battery_charge_total_sensor) { registers_G3[5].sensor = battery_charge_total_sensor; battery_charge_total_sensor_ = battery_charge_total_sensor; }
            void set_battery_discharge_today_sensor(sensor::Sensor *battery_discharge_today_sensor) { registers_G3[6].sensor = battery_discharge_today_sensor; battery_discharge_today_sensor_ = battery_discharge_today_sensor; }
            void set_battery_discharge_total_sensor(sensor::Sensor *battery_discharge_total_sensor) { registers_G3[7].sensor = battery_discharge_total_sensor; battery_discharge_total_sensor_ = battery_discharge_total_sensor; }
            void set_total_active_power_inverter_sensor(sensor::Sensor *total_active_power_inverter_sensor) { registers_G3[8].sensor = total_active_power_inverter_sensor; total_active_power_inverter_sensor_ = total_active_power_inverter_sensor; }
            void set_pv_voltage_1_sensor(sensor::Sensor *pv_voltage_1_sensor) { registers_G3[9].sensor = pv_voltage_1_sensor; pv_voltage_1_sensor_ = pv_voltage_1_sensor; }
            void set_pv_current_1_sensor(sensor::Sensor *pv_current_1_sensor) { registers_G3[10].sensor = pv_current_1_sensor; pv_current_1_sensor_ = pv_current_1_sensor; }
            void set_pv_power_1_sensor(sensor::Sensor *pv_power_1_sensor) { registers_G3[11].sensor = pv_power_1_sensor; pv_power_1_sensor_ = pv_power_1_sensor; }
            void set_pv_voltage_2_sensor(sensor::Sensor *pv_voltage_2_sensor) { registers_G3[12].sensor = pv_voltage_2_sensor; pv_voltage_2_sensor_ = pv_voltage_2_sensor; }
            void set_pv_current_2_sensor(sensor::Sensor *pv_current_2_sensor) { registers_G3[13].sensor = pv_current_2_sensor; pv_current_2_sensor_ = pv_current_2_sensor; }
            void set_pv_power_2_sensor(sensor::Sensor *pv_power_2_sensor) { registers_G3[14].sensor = pv_power_2_sensor; pv_power_2_sensor_ = pv_power_2_sensor; }
            void set_pv_power_total_sensor(sensor::Sensor *pv_power_total_sensor) { registers_G3[15].sensor = pv_power_total_sensor; pv_power_total_sensor_ = pv_power_total_sensor; }
            void set_battery_power_total_sensor(sensor::Sensor *battery_power_total_sensor) { registers_G3[16].sensor = battery_power_total_sensor; battery_power_total_sensor_ = battery_power_total_sensor; }
            void set_battery_state_of_charge_total_sensor(sensor::Sensor *battery_state_of_charge_total_sensor) { registers_G3[17].sensor = battery_state_of_charge_total_sensor; battery_state_of_charge_total_sensor_ = battery_state_of_charge_total_sensor; }
            void set_desired_grid_power_sensor(sensor::Sensor *desired_grid_power_sensor) { registers_G3[18].sensor = desired_grid_power_sensor; desired_grid_power_sensor_ = desired_grid_power_sensor; }
            void set_minimum_battery_power_sensor(sensor::Sensor *minimum_battery_power_sensor) { registers_G3[19].sensor = minimum_battery_power_sensor; minimum_battery_power_sensor_ = minimum_battery_power_sensor; }
            void set_maximum_battery_power_sensor(sensor::Sensor *maximum_battery_power_sensor) { registers_G3[20].sensor = maximum_battery_power_sensor; maximum_battery_power_sensor_ = maximum_battery_power_sensor; }
            void set_energy_storage_mode_sensor(sensor::Sensor *energy_storage_mode_sensor) { registers_G3[21].sensor = energy_storage_mode_sensor; energy_storage_mode_sensor_ = energy_storage_mode_sensor; }
            void set_battery_conf_id_sensor(sensor::Sensor *battery_conf_id_sensor) { registers_G3[22].sensor = battery_conf_id_sensor; battery_conf_id_sensor_ = battery_conf_id_sensor; }
            void set_battery_conf_address_sensor(sensor::Sensor *battery_conf_address_sensor) { registers_G3[23].sensor = battery_conf_address_sensor; battery_conf_address_sensor_ = battery_conf_address_sensor; }
            void set_battery_conf_protocol_sensor(sensor::Sensor *battery_conf_protocol_sensor) { registers_G3[24].sensor = battery_conf_protocol_sensor; battery_conf_protocol_sensor_ = battery_conf_protocol_sensor; }
            void set_battery_conf_voltage_nominal_sensor(sensor::Sensor *battery_conf_voltage_nominal_sensor) { registers_G3[25].sensor = battery_conf_voltage_nominal_sensor; battery_conf_voltage_nominal_sensor_ = battery_conf_voltage_nominal_sensor; }
            void set_battery_conf_voltage_over_sensor(sensor::Sensor *battery_conf_voltage_over_sensor) { registers_G3[26].sensor = battery_conf_voltage_over_sensor; battery_conf_voltage_over_sensor_ = battery_conf_voltage_over_sensor; }
            void set_battery_conf_voltage_charge_sensor(sensor::Sensor *battery_conf_voltage_charge_sensor) { registers_G3[27].sensor = battery_conf_voltage_charge_sensor; battery_conf_voltage_charge_sensor_ = battery_conf_voltage_charge_sensor; }
            void set_battery_conf_voltage_lack_sensor(sensor::Sensor *battery_conf_voltage_lack_sensor) { registers_G3[28].sensor = battery_conf_voltage_lack_sensor; battery_conf_voltage_lack_sensor_ = battery_conf_voltage_lack_sensor; }
            void set_battery_conf_voltage_discharge_stop_sensor(sensor::Sensor *battery_conf_voltage_discharge_stop_sensor) { registers_G3[29].sensor = battery_conf_voltage_discharge_stop_sensor; battery_conf_voltage_discharge_stop_sensor_ = battery_conf_voltage_discharge_stop_sensor; }
            void set_battery_conf_current_charge_limit_sensor(sensor::Sensor *battery_conf_current_charge_limit_sensor) { registers_G3[30].sensor = battery_conf_current_charge_limit_sensor; battery_conf_current_charge_limit_sensor_ = battery_conf_current_charge_limit_sensor; }
            void set_battery_conf_current_discharge_limit_sensor(sensor::Sensor *battery_conf_current_discharge_limit_sensor) { registers_G3[31].sensor = battery_conf_current_discharge_limit_sensor; battery_conf_current_discharge_limit_sensor_ = battery_conf_current_discharge_limit_sensor; }
            void set_battery_conf_depth_of_discharge_sensor(sensor::Sensor *battery_conf_depth_of_discharge_sensor) { registers_G3[32].sensor = battery_conf_depth_of_discharge_sensor; battery_conf_depth_of_discharge_sensor_ = battery_conf_depth_of_discharge_sensor; }
            void set_battery_conf_end_of_discharge_sensor(sensor::Sensor *battery_conf_end_of_discharge_sensor) { registers_G3[33].sensor = battery_conf_end_of_discharge_sensor; battery_conf_end_of_discharge_sensor_ = battery_conf_end_of_discharge_sensor; }
            void set_battery_conf_capacity_sensor(sensor::Sensor *battery_conf_capacity_sensor) { registers_G3[34].sensor = battery_conf_capacity_sensor; battery_conf_capacity_sensor_ = battery_conf_capacity_sensor; }
            void set_battery_conf_cell_type_sensor(sensor::Sensor *battery_conf_cell_type_sensor) { registers_G3[35].sensor = battery_conf_cell_type_sensor; battery_conf_cell_type_sensor_ = battery_conf_cell_type_sensor; }
            void set_battery_conf_eps_buffer_sensor(sensor::Sensor *battery_conf_eps_buffer_sensor) { registers_G3[36].sensor = battery_conf_eps_buffer_sensor; battery_conf_eps_buffer_sensor_ = battery_conf_eps_buffer_sensor; }
            void set_battery_conf_control_sensor(sensor::Sensor *battery_conf_control_sensor) { registers_G3[37].sensor = battery_conf_control_sensor; battery_conf_control_sensor_ = battery_conf_control_sensor; }
			void set_grid_frequency_sensor(sensor::Sensor *grid_frequency_sensor) { registers_G3[38].sensor = grid_frequency_sensor; grid_frequency_sensor_ = grid_frequency_sensor; }
			void set_grid_voltage_phase_r_sensor(sensor::Sensor *grid_voltage_phase_r_sensor) { registers_G3[39].sensor = grid_voltage_phase_r_sensor; grid_voltage_phase_r_sensor_ = grid_voltage_phase_r_sensor; }
			void set_grid_current_phase_r_sensor(sensor::Sensor *grid_current_phase_r_sensor) { registers_G3[40].sensor = grid_current_phase_r_sensor; grid_current_phase_r_sensor_ = grid_current_phase_r_sensor; }
			void set_grid_power_phase_r_sensor(sensor::Sensor *grid_power_phase_r_sensor) { registers_G3[41].sensor = grid_power_phase_r_sensor; grid_power_phase_r_sensor_ = grid_power_phase_r_sensor; }
			void set_grid_voltage_phase_s_sensor(sensor::Sensor *grid_voltage_phase_s_sensor) { registers_G3[42].sensor = grid_voltage_phase_s_sensor; grid_voltage_phase_s_sensor_ = grid_voltage_phase_s_sensor; }
			void set_grid_current_phase_s_sensor(sensor::Sensor *grid_current_phase_s_sensor) { registers_G3[43].sensor = grid_current_phase_s_sensor; grid_current_phase_s_sensor_ = grid_current_phase_s_sensor; }
			void set_grid_power_phase_s_sensor(sensor::Sensor *grid_power_phase_s_sensor) { registers_G3[44].sensor = grid_power_phase_s_sensor; grid_power_phase_s_sensor_ = grid_power_phase_s_sensor; }
			void set_grid_voltage_phase_t_sensor(sensor::Sensor *grid_voltage_phase_t_sensor) { registers_G3[45].sensor = grid_voltage_phase_t_sensor; grid_voltage_phase_t_sensor_ = grid_voltage_phase_t_sensor; }
			void set_grid_current_phase_t_sensor(sensor::Sensor *grid_current_phase_t_sensor) { registers_G3[46].sensor = grid_current_phase_t_sensor; grid_current_phase_t_sensor_ = grid_current_phase_t_sensor; }
			void set_grid_power_phase_t_sensor(sensor::Sensor *grid_power_phase_t_sensor) { registers_G3[47].sensor = grid_power_phase_t_sensor; grid_power_phase_t_sensor_ = grid_power_phase_t_sensor; }
			void set_off_grid_power_total_sensor(sensor::Sensor *off_grid_power_total_sensor) { registers_G3[48].sensor = off_grid_power_total_sensor; off_grid_power_total_sensor_ = off_grid_power_total_sensor; }
			void set_off_grid_frequency_sensor(sensor::Sensor *off_grid_frequency_sensor) { registers_G3[49].sensor = off_grid_frequency_sensor; off_grid_frequency_sensor_ = off_grid_frequency_sensor; }
			void set_off_grid_voltage_phase_r_sensor(sensor::Sensor *off_grid_voltage_phase_r_sensor) { registers_G3[50].sensor = off_grid_voltage_phase_r_sensor; off_grid_voltage_phase_r_sensor_ = off_grid_voltage_phase_r_sensor; }
			void set_off_grid_current_phase_r_sensor(sensor::Sensor *off_grid_current_phase_r_sensor) { registers_G3[51].sensor = off_grid_current_phase_r_sensor; off_grid_current_phase_r_sensor_ = off_grid_current_phase_r_sensor; }
			void set_off_grid_power_phase_r_sensor(sensor::Sensor *off_grid_power_phase_r_sensor) { registers_G3[52].sensor = off_grid_power_phase_r_sensor; off_grid_power_phase_r_sensor_ = off_grid_power_phase_r_sensor; }
			void set_off_grid_voltage_phase_s_sensor(sensor::Sensor *off_grid_voltage_phase_s_sensor) { registers_G3[53].sensor = off_grid_voltage_phase_s_sensor; off_grid_voltage_phase_s_sensor_ = off_grid_voltage_phase_s_sensor; }
			void set_off_grid_current_phase_s_sensor(sensor::Sensor *off_grid_current_phase_s_sensor) { registers_G3[54].sensor = off_grid_current_phase_s_sensor; off_grid_current_phase_s_sensor_ = off_grid_current_phase_s_sensor; }
			void set_off_grid_power_phase_s_sensor(sensor::Sensor *off_grid_power_phase_s_sensor) { registers_G3[55].sensor = off_grid_power_phase_s_sensor; off_grid_power_phase_s_sensor_ = off_grid_power_phase_s_sensor; }
			void set_off_grid_voltage_phase_t_sensor(sensor::Sensor *off_grid_voltage_phase_t_sensor) { registers_G3[56].sensor = off_grid_voltage_phase_t_sensor; off_grid_voltage_phase_t_sensor_ = off_grid_voltage_phase_t_sensor; }
			void set_off_grid_current_phase_t_sensor(sensor::Sensor *off_grid_current_phase_t_sensor) { registers_G3[57].sensor = off_grid_current_phase_t_sensor; off_grid_current_phase_t_sensor_ = off_grid_current_phase_t_sensor; }
			void set_off_grid_power_phase_t_sensor(sensor::Sensor *off_grid_power_phase_t_sensor) { registers_G3[58].sensor = off_grid_power_phase_t_sensor; off_grid_power_phase_t_sensor_ = off_grid_power_phase_t_sensor; }

            // Set update intervals for sensors

            void set_pv_generation_today_sensor_update_interval(int pv_generation_today_sensor_update_interval) { registers_G3[0].update_interval = pv_generation_today_sensor_update_interval; pv_generation_today_sensor_update_interval_ = pv_generation_today_sensor_update_interval; }
            void set_pv_generation_total_sensor_update_interval(int pv_generation_total_sensor_update_interval) { registers_G3[1].update_interval = pv_generation_total_sensor_update_interval; pv_generation_total_sensor_update_interval_ = pv_generation_total_sensor_update_interval; }
            void set_load_consumption_today_sensor_update_interval(int load_consumption_today_sensor_update_interval) { registers_G3[2].update_interval = load_consumption_today_sensor_update_interval; load_consumption_today_sensor_update_interval_ = load_consumption_today_sensor_update_interval; }
            void set_load_consumption_total_sensor_update_interval(int load_consumption_total_sensor_update_interval) { registers_G3[3].update_interval = load_consumption_total_sensor_update_interval; load_consumption_total_sensor_update_interval_ = load_consumption_total_sensor_update_interval; }
            void set_battery_charge_today_sensor_update_interval(int battery_charge_today_sensor_update_interval) { registers_G3[4].update_interval = battery_charge_today_sensor_update_interval; battery_charge_today_sensor_update_interval_ = battery_charge_today_sensor_update_interval; }
            void set_battery_charge_total_sensor_update_interval(int battery_charge_total_sensor_update_interval) { registers_G3[5].update_interval = battery_charge_total_sensor_update_interval; battery_charge_total_sensor_update_interval_ = battery_charge_total_sensor_update_interval; }
            void set_battery_discharge_today_sensor_update_interval(int battery_discharge_today_sensor_update_interval) { registers_G3[6].update_interval = battery_discharge_today_sensor_update_interval; battery_discharge_today_sensor_update_interval_ = battery_discharge_today_sensor_update_interval; }
            void set_battery_discharge_total_sensor_update_interval(int battery_discharge_total_sensor_update_interval) { registers_G3[7].update_interval = battery_discharge_total_sensor_update_interval; battery_discharge_total_sensor_update_interval_ = battery_discharge_total_sensor_update_interval; }
            void set_total_active_power_inverter_sensor_update_interval(int total_active_power_inverter_sensor_update_interval) { registers_G3[8].update_interval = total_active_power_inverter_sensor_update_interval; total_active_power_inverter_sensor_update_interval_ = total_active_power_inverter_sensor_update_interval; }
            void set_pv_voltage_1_sensor_update_interval(int pv_voltage_1_sensor_update_interval) { registers_G3[9].update_interval = pv_voltage_1_sensor_update_interval; pv_voltage_1_sensor_update_interval_ = pv_voltage_1_sensor_update_interval; }
            void set_pv_current_1_sensor_update_interval(int pv_current_1_sensor_update_interval) { registers_G3[10].update_interval = pv_current_1_sensor_update_interval; pv_current_1_sensor_update_interval_ = pv_current_1_sensor_update_interval; }
            void set_pv_power_1_sensor_update_interval(int pv_power_1_sensor_update_interval) { registers_G3[11].update_interval = pv_power_1_sensor_update_interval; pv_power_1_sensor_update_interval_ = pv_power_1_sensor_update_interval; }
            void set_pv_voltage_2_sensor_update_interval(int pv_voltage_2_sensor_update_interval) { registers_G3[12].update_interval = pv_voltage_2_sensor_update_interval; pv_voltage_2_sensor_update_interval_ = pv_voltage_2_sensor_update_interval; }
            void set_pv_current_2_sensor_update_interval(int pv_current_2_sensor_update_interval) { registers_G3[13].update_interval = pv_current_2_sensor_update_interval; pv_current_2_sensor_update_interval_ = pv_current_2_sensor_update_interval; }
            void set_pv_power_2_sensor_update_interval(int pv_power_2_sensor_update_interval) { registers_G3[14].update_interval = pv_power_2_sensor_update_interval; pv_power_2_sensor_update_interval_ = pv_power_2_sensor_update_interval; }
            void set_pv_power_total_sensor_update_interval(int pv_power_total_sensor_update_interval) { registers_G3[15].update_interval = pv_power_total_sensor_update_interval; pv_power_total_sensor_update_interval_ = pv_power_total_sensor_update_interval; }
            void set_battery_power_total_sensor_update_interval(int battery_power_total_sensor_update_interval) { registers_G3[16].update_interval = battery_power_total_sensor_update_interval; battery_power_total_sensor_update_interval_ = battery_power_total_sensor_update_interval; }
            void set_battery_state_of_charge_total_sensor_update_interval(int battery_state_of_charge_total_sensor_update_interval) { registers_G3[17].update_interval = battery_state_of_charge_total_sensor_update_interval; battery_state_of_charge_total_sensor_update_interval_ = battery_state_of_charge_total_sensor_update_interval; }
            void set_desired_grid_power_sensor_update_interval(int desired_grid_power_sensor_update_interval) { registers_G3[18].update_interval = desired_grid_power_sensor_update_interval; desired_grid_power_sensor_update_interval_ = desired_grid_power_sensor_update_interval; }
            void set_minimum_battery_power_sensor_update_interval(int minimum_battery_power_sensor_update_interval) { registers_G3[19].update_interval = minimum_battery_power_sensor_update_interval; minimum_battery_power_sensor_update_interval_ = minimum_battery_power_sensor_update_interval; }
            void set_maximum_battery_power_sensor_update_interval(int maximum_battery_power_sensor_update_interval) { registers_G3[20].update_interval = maximum_battery_power_sensor_update_interval; maximum_battery_power_sensor_update_interval_ = maximum_battery_power_sensor_update_interval; }
            void set_energy_storage_mode_sensor_update_interval(int energy_storage_mode_sensor_update_interval) { registers_G3[21].update_interval = energy_storage_mode_sensor_update_interval; energy_storage_mode_sensor_update_interval_ = energy_storage_mode_sensor_update_interval; }
            void set_battery_conf_id_sensor_update_interval(int battery_conf_id_sensor_update_interval) { registers_G3[22].update_interval = battery_conf_id_sensor_update_interval; battery_conf_id_sensor_update_interval_ = battery_conf_id_sensor_update_interval; }
            void set_battery_conf_address_sensor_update_interval(int battery_conf_address_sensor_update_interval) { registers_G3[23].update_interval = battery_conf_address_sensor_update_interval; battery_conf_address_sensor_update_interval_ = battery_conf_address_sensor_update_interval; }
            void set_battery_conf_protocol_sensor_update_interval(int battery_conf_protocol_sensor_update_interval) { registers_G3[24].update_interval = battery_conf_protocol_sensor_update_interval; battery_conf_protocol_sensor_update_interval_ = battery_conf_protocol_sensor_update_interval; }
            void set_battery_conf_voltage_nominal_sensor_update_interval(int battery_conf_voltage_nominal_sensor_update_interval) { registers_G3[25].update_interval = battery_conf_voltage_nominal_sensor_update_interval; battery_conf_voltage_nominal_sensor_update_interval_ = battery_conf_voltage_nominal_sensor_update_interval; }
            void set_battery_conf_voltage_over_sensor_update_interval(int battery_conf_voltage_over_sensor_update_interval) { registers_G3[26].update_interval = battery_conf_voltage_over_sensor_update_interval; battery_conf_voltage_over_sensor_update_interval_ = battery_conf_voltage_over_sensor_update_interval; }
            void set_battery_conf_voltage_charge_sensor_update_interval(int battery_conf_voltage_charge_sensor_update_interval) { registers_G3[27].update_interval = battery_conf_voltage_charge_sensor_update_interval; battery_conf_voltage_charge_sensor_update_interval_ = battery_conf_voltage_charge_sensor_update_interval; }
            void set_battery_conf_voltage_lack_sensor_update_interval(int battery_conf_voltage_lack_sensor_update_interval) { registers_G3[28].update_interval = battery_conf_voltage_lack_sensor_update_interval; battery_conf_voltage_lack_sensor_update_interval_ = battery_conf_voltage_lack_sensor_update_interval; }
            void set_battery_conf_voltage_discharge_stop_sensor_update_interval(int battery_conf_voltage_discharge_stop_sensor_update_interval) { registers_G3[29].update_interval = battery_conf_voltage_discharge_stop_sensor_update_interval; battery_conf_voltage_discharge_stop_sensor_update_interval_ = battery_conf_voltage_discharge_stop_sensor_update_interval; }
            void set_battery_conf_current_charge_limit_sensor_update_interval(int battery_conf_current_charge_limit_sensor_update_interval) { registers_G3[30].update_interval = battery_conf_current_charge_limit_sensor_update_interval; battery_conf_current_charge_limit_sensor_update_interval_ = battery_conf_current_charge_limit_sensor_update_interval; }
            void set_battery_conf_current_discharge_limit_sensor_update_interval(int battery_conf_current_discharge_limit_sensor_update_interval) { registers_G3[31].update_interval = battery_conf_current_discharge_limit_sensor_update_interval; battery_conf_current_discharge_limit_sensor_update_interval_ = battery_conf_current_discharge_limit_sensor_update_interval; }
            void set_battery_conf_depth_of_discharge_sensor_update_interval(int battery_conf_depth_of_discharge_sensor_update_interval) { registers_G3[32].update_interval = battery_conf_depth_of_discharge_sensor_update_interval; battery_conf_depth_of_discharge_sensor_update_interval_ = battery_conf_depth_of_discharge_sensor_update_interval; }
            void set_battery_conf_end_of_discharge_sensor_update_interval(int battery_conf_end_of_discharge_sensor_update_interval) { registers_G3[33].update_interval = battery_conf_end_of_discharge_sensor_update_interval; battery_conf_end_of_discharge_sensor_update_interval_ = battery_conf_end_of_discharge_sensor_update_interval; }
            void set_battery_conf_capacity_sensor_update_interval(int battery_conf_capacity_sensor_update_interval) { registers_G3[34].update_interval = battery_conf_capacity_sensor_update_interval; battery_conf_capacity_sensor_update_interval_ = battery_conf_capacity_sensor_update_interval; }
            void set_battery_conf_cell_type_sensor_update_interval(int battery_conf_cell_type_sensor_update_interval) { registers_G3[35].update_interval = battery_conf_cell_type_sensor_update_interval; battery_conf_cell_type_sensor_update_interval_ = battery_conf_cell_type_sensor_update_interval; }
            void set_battery_conf_eps_buffer_sensor_update_interval(int battery_conf_eps_buffer_sensor_update_interval) { registers_G3[36].update_interval = battery_conf_eps_buffer_sensor_update_interval; battery_conf_eps_buffer_sensor_update_interval_ = battery_conf_eps_buffer_sensor_update_interval; }
            void set_battery_conf_control_sensor_update_interval(int battery_conf_control_sensor_update_interval) { registers_G3[37].update_interval = battery_conf_control_sensor_update_interval; battery_conf_control_sensor_update_interval_ = battery_conf_control_sensor_update_interval; }
			void set_grid_frequency_sensor_update_interval(int grid_frequency_sensor_update_interval) { registers_G3[38].update_interval = grid_frequency_sensor_update_interval; grid_frequency_sensor_update_interval_ = grid_frequency_sensor_update_interval; }
			void set_grid_voltage_phase_r_sensor_update_interval(int grid_voltage_phase_r_sensor_update_interval) { registers_G3[39].update_interval = grid_voltage_phase_r_sensor_update_interval; grid_voltage_phase_r_sensor_update_interval_ = grid_voltage_phase_r_sensor_update_interval; }
			void set_grid_current_phase_r_sensor_update_interval(int grid_current_phase_r_sensor_update_interval) { registers_G3[40].update_interval = grid_current_phase_r_sensor_update_interval; grid_current_phase_r_sensor_update_interval_ = grid_current_phase_r_sensor_update_interval; }
			void set_grid_power_phase_r_sensor_update_interval(int grid_power_phase_r_sensor_update_interval) { registers_G3[41].update_interval = grid_power_phase_r_sensor_update_interval; grid_power_phase_r_sensor_update_interval_ = grid_power_phase_r_sensor_update_interval; }
			void set_grid_voltage_phase_s_sensor_update_interval(int grid_voltage_phase_s_sensor_update_interval) { registers_G3[42].update_interval = grid_voltage_phase_s_sensor_update_interval; grid_voltage_phase_s_sensor_update_interval_ = grid_voltage_phase_s_sensor_update_interval; }
			void set_grid_current_phase_s_sensor_update_interval(int grid_current_phase_s_sensor_update_interval) { registers_G3[43].update_interval = grid_current_phase_s_sensor_update_interval; grid_current_phase_s_sensor_update_interval_ = grid_current_phase_s_sensor_update_interval; }
			void set_grid_power_phase_s_sensor_update_interval(int grid_power_phase_s_sensor_update_interval) { registers_G3[44].update_interval = grid_power_phase_s_sensor_update_interval; grid_power_phase_s_sensor_update_interval_ = grid_power_phase_s_sensor_update_interval; }
			void set_grid_voltage_phase_t_sensor_update_interval(int grid_voltage_phase_t_sensor_update_interval) { registers_G3[45].update_interval = grid_voltage_phase_t_sensor_update_interval; grid_voltage_phase_t_sensor_update_interval_ = grid_voltage_phase_t_sensor_update_interval; }
			void set_grid_current_phase_t_sensor_update_interval(int grid_current_phase_t_sensor_update_interval) { registers_G3[46].update_interval = grid_current_phase_t_sensor_update_interval; grid_current_phase_t_sensor_update_interval_ = grid_current_phase_t_sensor_update_interval; }
			void set_grid_power_phase_t_sensor_update_interval(int grid_power_phase_t_sensor_update_interval) { registers_G3[47].update_interval = grid_power_phase_t_sensor_update_interval; grid_power_phase_t_sensor_update_interval_ = grid_power_phase_t_sensor_update_interval; }
			void set_off_grid_power_total_sensor_update_interval(int off_grid_power_total_sensor_update_interval) { registers_G3[48].update_interval = off_grid_power_total_sensor_update_interval; off_grid_power_total_sensor_update_interval_ = off_grid_power_total_sensor_update_interval; }
			void set_off_grid_frequency_sensor_update_interval(int off_grid_frequency_sensor_update_interval) { registers_G3[49].update_interval = off_grid_frequency_sensor_update_interval; off_grid_frequency_sensor_update_interval_ = off_grid_frequency_sensor_update_interval; }
			void set_off_grid_voltage_phase_r_sensor_update_interval(int off_grid_voltage_phase_r_sensor_update_interval) { registers_G3[50].update_interval = off_grid_voltage_phase_r_sensor_update_interval; off_grid_voltage_phase_r_sensor_update_interval_ = off_grid_voltage_phase_r_sensor_update_interval; }
			void set_off_grid_current_phase_r_sensor_update_interval(int off_grid_current_phase_r_sensor_update_interval) { registers_G3[51].update_interval = off_grid_current_phase_r_sensor_update_interval; off_grid_current_phase_r_sensor_update_interval_ = off_grid_current_phase_r_sensor_update_interval; }
			void set_off_grid_power_phase_r_sensor_update_interval(int off_grid_power_phase_r_sensor_update_interval) { registers_G3[52].update_interval = off_grid_power_phase_r_sensor_update_interval; off_grid_power_phase_r_sensor_update_interval_ = off_grid_power_phase_r_sensor_update_interval; }
			void set_off_grid_voltage_phase_s_sensor_update_interval(int off_grid_voltage_phase_s_sensor_update_interval) { registers_G3[53].update_interval = off_grid_voltage_phase_s_sensor_update_interval; off_grid_voltage_phase_s_sensor_update_interval_ = off_grid_voltage_phase_s_sensor_update_interval; }
			void set_off_grid_current_phase_s_sensor_update_interval(int off_grid_current_phase_s_sensor_update_interval) { registers_G3[54].update_interval = off_grid_current_phase_s_sensor_update_interval; off_grid_current_phase_s_sensor_update_interval_ = off_grid_current_phase_s_sensor_update_interval; }
			void set_off_grid_power_phase_s_sensor_update_interval(int off_grid_power_phase_s_sensor_update_interval) { registers_G3[55].update_interval = off_grid_power_phase_s_sensor_update_interval; off_grid_power_phase_s_sensor_update_interval_ = off_grid_power_phase_s_sensor_update_interval; }
			void set_off_grid_voltage_phase_t_sensor_update_interval(int off_grid_voltage_phase_t_sensor_update_interval) { registers_G3[56].update_interval = off_grid_voltage_phase_t_sensor_update_interval; off_grid_voltage_phase_t_sensor_update_interval_ = off_grid_voltage_phase_t_sensor_update_interval; }
			void set_off_grid_current_phase_t_sensor_update_interval(int off_grid_current_phase_t_sensor_update_interval) { registers_G3[57].update_interval = off_grid_current_phase_t_sensor_update_interval; off_grid_current_phase_t_sensor_update_interval_ = off_grid_current_phase_t_sensor_update_interval; }
			void set_off_grid_power_phase_t_sensor_update_interval(int off_grid_power_phase_t_sensor_update_interval) { registers_G3[58].update_interval = off_grid_power_phase_t_sensor_update_interval; off_grid_power_phase_t_sensor_update_interval_ = off_grid_power_phase_t_sensor_update_interval; }

			void set_desired_grid_power_sensor_default_value(int64_t default_value) { registers_G3[18].default_value.int64_value = default_value; registers_G3[18].is_default_value_set = true; }
			void set_minimum_battery_power_sensor_default_value(int64_t default_value) { registers_G3[19].default_value.int64_value = default_value; registers_G3[19].is_default_value_set = true; }
			void set_maximum_battery_power_sensor_default_value(int64_t default_value) { registers_G3[20].default_value.int64_value = default_value; registers_G3[20].is_default_value_set = true; }
			void set_energy_storage_mode_sensor_default_value(int64_t default_value) { registers_G3[21].default_value.int64_value = default_value; registers_G3[21].is_default_value_set = true; }
			void set_battery_conf_id_sensor_default_value(int64_t default_value) { registers_G3[22].default_value.int64_value = default_value; registers_G3[22].is_default_value_set = true; }
			void set_battery_conf_address_sensor_default_value(int64_t default_value) { registers_G3[23].default_value.int64_value = default_value; registers_G3[23].is_default_value_set = true; }
			void set_battery_conf_protocol_sensor_default_value(int64_t default_value) { registers_G3[24].default_value.int64_value = default_value; registers_G3[24].is_default_value_set = true; }
			void set_battery_conf_voltage_nominal_sensor_default_value(float default_value) { registers_G3[25].default_value.int64_value = (int64_t) (default_value / registers_G3[25].scale); registers_G3[25].is_default_value_set = true; }
			void set_battery_conf_voltage_over_sensor_default_value(float default_value) { registers_G3[26].default_value.int64_value = (int64_t) (default_value / registers_G3[26].scale); registers_G3[26].is_default_value_set = true; }
			void set_battery_conf_voltage_charge_sensor_default_value(float default_value) { registers_G3[27].default_value.int64_value = (int64_t) (default_value / registers_G3[27].scale); registers_G3[27].is_default_value_set = true; }
			void set_battery_conf_voltage_lack_sensor_default_value(float default_value) { registers_G3[28].default_value.int64_value = (int64_t) (default_value / registers_G3[28].scale); registers_G3[28].is_default_value_set = true; }
			void set_battery_conf_voltage_discharge_stop_sensor_default_value(float default_value) { registers_G3[29].default_value.int64_value = (int64_t) (default_value / registers_G3[29].scale); registers_G3[29].is_default_value_set = true; }
			void set_battery_conf_current_charge_limit_sensor_default_value(float default_value) { registers_G3[30].default_value.int64_value = (int64_t) (default_value / registers_G3[30].scale); registers_G3[30].is_default_value_set = true; }
			void set_battery_conf_current_discharge_limit_sensor_default_value(float default_value) { registers_G3[31].default_value.int64_value = (int64_t) (default_value / registers_G3[31].scale); registers_G3[31].is_default_value_set = true; }
			void set_battery_conf_depth_of_discharge_sensor_default_value(int64_t default_value) { registers_G3[32].default_value.int64_value = default_value; registers_G3[32].is_default_value_set = true; }
			void set_battery_conf_end_of_discharge_sensor_default_value(int64_t default_value) { registers_G3[33].default_value.int64_value = default_value; registers_G3[33].is_default_value_set = true; }
			void set_battery_conf_capacity_sensor_default_value(int64_t default_value) { registers_G3[34].default_value.int64_value = default_value; registers_G3[34].is_default_value_set = true; }
			void set_battery_conf_cell_type_sensor_default_value(int64_t default_value) { registers_G3[35].default_value.int64_value = default_value; registers_G3[35].is_default_value_set = true; }
			void set_battery_conf_eps_buffer_sensor_default_value(int64_t default_value) { registers_G3[36].default_value.int64_value = default_value; registers_G3[36].is_default_value_set = true; }

			void set_desired_grid_power_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[18].enforce_default_value = enforce_default_value; }
			void set_minimum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[19].enforce_default_value = enforce_default_value; }
			void set_maximum_battery_power_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[20].enforce_default_value = enforce_default_value; }
			void set_energy_storage_mode_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[21].enforce_default_value = enforce_default_value; }
			void set_battery_conf_id_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[22].enforce_default_value = enforce_default_value; }
			void set_battery_conf_address_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[23].enforce_default_value = enforce_default_value; }
			void set_battery_conf_protocol_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[24].enforce_default_value = enforce_default_value; }
			void set_battery_conf_voltage_nominal_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[25].enforce_default_value = enforce_default_value; }
			void set_battery_conf_voltage_over_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[26].enforce_default_value = enforce_default_value; }
			void set_battery_conf_voltage_charge_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[27].enforce_default_value = enforce_default_value; }
			void set_battery_conf_voltage_lack_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[28].enforce_default_value = enforce_default_value; }
			void set_battery_conf_voltage_discharge_stop_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[29].enforce_default_value = enforce_default_value; }
			void set_battery_conf_current_charge_limit_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[30].enforce_default_value = enforce_default_value; }
			void set_battery_conf_current_discharge_limit_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[31].enforce_default_value = enforce_default_value; }
			void set_battery_conf_depth_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[32].enforce_default_value = enforce_default_value; }
			void set_battery_conf_end_of_discharge_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[33].enforce_default_value = enforce_default_value; }
			void set_battery_conf_capacity_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[34].enforce_default_value = enforce_default_value; }
			void set_battery_conf_cell_type_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[35].enforce_default_value = enforce_default_value; }
			void set_battery_conf_eps_buffer_sensor_enforce_default_value(bool enforce_default_value) { registers_G3[36].enforce_default_value = enforce_default_value; }

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