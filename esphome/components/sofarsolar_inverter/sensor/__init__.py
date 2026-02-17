import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv

from .. import CONF_SOFARSOLAR_INVERTER_ID, SOFARSOLAR_INVERTER_COMPONENT_SCHEMA

from esphome.const import (
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_ENERGY_STORAGE,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_BATTERY_CHARGING,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_WATT,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_HERTZ,
    UNIT_WATT_HOURS,
    UNIT_KILOWATT_HOURS,
    UNIT_PERCENT,
    UNIT_SECOND,
    UNIT_CELSIUS,

    UNIT_EMPTY
)

DEPENDENCIES = ["modbus"]

CONF_PV_GENERATION_TODAY = "pv_generation_today"
CONF_PV_GENERATION_TOTAL = "pv_generation_total"
CONF_LOAD_CONSUMPTION_TODAY = "load_consumption_today"
CONF_LOAD_CONSUMPTION_TOTAL = "load_consumption_total"
CONF_BATTERY_CHARGE_TODAY = "battery_charge_today"
CONF_BATTERY_CHARGE_TOTAL = "battery_charge_total"
CONF_BATTERY_DISCHARGE_TODAY = "battery_discharge_today"
CONF_BATTERY_DISCHARGE_TOTAL = "battery_discharge_total"
CONF_TOTAL_ACTIVE_POWER_INVERTER = "total_active_power_inverter"
CONF_PV_VOLTAGE_1 = "pv_voltage_1"
CONF_PV_CURRENT_1 = "pv_current_1"
CONF_PV_POWER_1 = "pv_power_1"
CONF_PV_VOLTAGE_2 = "pv_voltage_2"
CONF_PV_CURRENT_2 = "pv_current_2"
CONF_PV_POWER_2 = "pv_power_2"
CONF_PV_POWER_TOTAL = "pv_power_total"

CONF_BATTERY_VOLTAGE_1 = "battery_voltage_1"
CONF_BATTERY_CURRENT_1 = "battery_current_1"
CONF_BATTERY_POWER_1 = "battery_power_1"
CONF_BATTERY_TEMPERATURE_ENV_1 = "battery_temperature_environment_1"
CONF_BATTERY_STATE_OF_CHARGE_1 = "battery_state_of_charge_1"
CONF_BATTERY_STATE_OF_HEALTH_1 = "battery_state_of_health_1"
CONF_BATTERY_CHARGE_CYCLE_1 = "battery_charge_cycle_1"

CONF_BATTERY_VOLTAGE_2 = "battery_voltage_2"
CONF_BATTERY_CURRENT_2 = "battery_current_2"
CONF_BATTERY_POWER_2 = "battery_power_2"
CONF_BATTERY_TEMPERATURE_ENV_2 = "battery_temperature_environment_2"
CONF_BATTERY_STATE_OF_CHARGE_2 = "battery_state_of_charge_2"
CONF_BATTERY_STATE_OF_HEALTH_2 = "battery_state_of_health_2"
CONF_BATTERY_CHARGE_CYCLE_2 = "battery_charge_cycle_2"

CONF_BATTERY_VOLTAGE_3 = "battery_voltage_3"
CONF_BATTERY_CURRENT_3 = "battery_current_3"
CONF_BATTERY_POWER_3 = "battery_power_3"
CONF_BATTERY_TEMPERATURE_ENV_3 = "battery_temperature_environment_3"
CONF_BATTERY_STATE_OF_CHARGE_3 = "battery_state_of_charge_3"
CONF_BATTERY_STATE_OF_HEALTH_3 = "battery_state_of_health_3"
CONF_BATTERY_CHARGE_CYCLE_3 = "battery_charge_cycle_3"

CONF_BATTERY_VOLTAGE_4 = "battery_voltage_4"
CONF_BATTERY_CURRENT_4 = "battery_current_4"
CONF_BATTERY_POWER_4 = "battery_power_4"
CONF_BATTERY_TEMPERATURE_ENV_4 = "battery_temperature_environment_4"
CONF_BATTERY_STATE_OF_CHARGE_4 = "battery_state_of_charge_4"
CONF_BATTERY_STATE_OF_HEALTH_4 = "battery_state_of_health_4"
CONF_BATTERY_CHARGE_CYCLE_4 = "battery_charge_cycle_4"

CONF_BATTERY_VOLTAGE_5 = "battery_voltage_5"
CONF_BATTERY_CURRENT_5 = "battery_current_5"
CONF_BATTERY_POWER_5 = "battery_power_5"
CONF_BATTERY_TEMPERATURE_ENV_5 = "battery_temperature_environment_5"
CONF_BATTERY_STATE_OF_CHARGE_5 = "battery_state_of_charge_5"
CONF_BATTERY_STATE_OF_HEALTH_5 = "battery_state_of_health_5"
CONF_BATTERY_CHARGE_CYCLE_5 = "battery_charge_cycle_5"

CONF_BATTERY_VOLTAGE_6 = "battery_voltage_6"
CONF_BATTERY_CURRENT_6 = "battery_current_6"
CONF_BATTERY_POWER_6 = "battery_power_6"
CONF_BATTERY_TEMPERATURE_ENV_6 = "battery_temperature_environment_6"
CONF_BATTERY_STATE_OF_CHARGE_6 = "battery_state_of_charge_6"
CONF_BATTERY_STATE_OF_HEALTH_6 = "battery_state_of_health_6"
CONF_BATTERY_CHARGE_CYCLE_6 = "battery_charge_cycle_6"

CONF_BATTERY_VOLTAGE_7 = "battery_voltage_7"
CONF_BATTERY_CURRENT_7 = "battery_current_7"
CONF_BATTERY_POWER_7 = "battery_power_7"
CONF_BATTERY_TEMPERATURE_ENV_7 = "battery_temperature_environment_7"
CONF_BATTERY_STATE_OF_CHARGE_7 = "battery_state_of_charge_7"
CONF_BATTERY_STATE_OF_HEALTH_7 = "battery_state_of_health_7"
CONF_BATTERY_CHARGE_CYCLE_7 = "battery_charge_cycle_7"

CONF_BATTERY_VOLTAGE_8 = "battery_voltage_8"
CONF_BATTERY_CURRENT_8 = "battery_current_8"
CONF_BATTERY_POWER_8 = "battery_power_8"
CONF_BATTERY_TEMPERATURE_ENV_8 = "battery_temperature_environment_8"
CONF_BATTERY_STATE_OF_CHARGE_8 = "battery_state_of_charge_8"
CONF_BATTERY_STATE_OF_HEALTH_8 = "battery_state_of_health_8"
CONF_BATTERY_CHARGE_CYCLE_8 = "battery_charge_cycle_8"

CONF_BATTERY_POWER_TOTAL = "battery_power_total"
CONF_BATTERY_STATE_OF_CHARGE_TOTAL = "battery_state_of_charge_total"
CONF_DESIRED_GRID_POWER = "desired_grid_power"
CONF_MINIMUM_BATTERY_POWER = "minimum_battery_power"
CONF_MAXIMUM_BATTERY_POWER = "maximum_battery_power"
CONF_ENERGY_STORAGE_MODE = "energy_storage_mode"
CONF_BATTERY_CONF_ID = "battery_conf_id"
CONF_BATTERY_CONF_ADDRESS = "battery_conf_address"
CONF_BATTERY_CONF_PROTOCOL = "battery_conf_protocol"
CONF_BATTERY_CONF_VOLTAGE_NOMINAL = "battery_conf_voltage_nominal"
CONF_BATTERY_CONF_VOLTAGE_OVER = "battery_conf_voltage_over"
CONF_BATTERY_CONF_VOLTAGE_CHARGE = "battery_conf_voltage_charge"
CONF_BATTERY_CONF_VOLTAGE_LACK = "battery_conf_voltage_lack"
CONF_BATTERY_CONF_VOLTAGE_DISCHARGE_STOP = "battery_conf_voltage_discharge_stop"
CONF_BATTERY_CONF_CURRENT_CHARGE_LIMIT = "battery_conf_current_charge_limit"
CONF_BATTERY_CONF_CURRENT_DISCHARGE_LIMIT = "battery_conf_current_discharge_limit"
CONF_BATTERY_CONF_DEPTH_OF_DISCHARGE = "battery_conf_depth_of_discharge"
CONF_BATTERY_CONF_END_OF_DISCHARGE = "battery_conf_end_of_discharge"
CONF_BATTERY_CONF_CAPACITY = "battery_conf_capacity"
CONF_BATTERY_CONF_CELL_TYPE = "battery_conf_cell_type"
CONF_BATTERY_CONF_EPS_BUFFER = "battery_conf_eps_buffer"
CONF_BATTERY_CONF_CONTROL = "battery_conf_control"
CONF_GRID_FREQUENCY = "grid_frequency"
CONF_GRID_VOLTAGE_PHASE_R = "grid_voltage_phase_r"
CONF_GRID_CURRENT_PHASE_R = "grid_current_phase_r"
CONF_GRID_POWER_PHASE_R = "grid_power_phase_r"
CONF_GRID_VOLTAGE_PHASE_S = "grid_voltage_phase_s"
CONF_GRID_CURRENT_PHASE_S = "grid_current_phase_s"
CONF_GRID_POWER_PHASE_S = "grid_power_phase_s"
CONF_GRID_VOLTAGE_PHASE_T = "grid_voltage_phase_t"
CONF_GRID_CURRENT_PHASE_T = "grid_current_phase_t"
CONF_GRID_POWER_PHASE_T = "grid_power_phase_t"
CONF_OFF_GRID_POWER_TOTAL = "off_grid_power_total"
CONF_OFF_GRID_FREQUENCY = "off_grid_frequency"
CONF_OFF_GRID_VOLTAGE_PHASE_R = "off_grid_voltage_phase_r"
CONF_OFF_GRID_CURRENT_PHASE_R = "off_grid_current_phase_r"
CONF_OFF_GRID_POWER_PHASE_R = "off_grid_power_phase_r"
CONF_OFF_GRID_VOLTAGE_PHASE_S = "off_grid_voltage_phase_s"
CONF_OFF_GRID_CURRENT_PHASE_S = "off_grid_current_phase_s"
CONF_OFF_GRID_POWER_PHASE_S = "off_grid_power_phase_s"
CONF_OFF_GRID_VOLTAGE_PHASE_T = "off_grid_voltage_phase_t"
CONF_OFF_GRID_CURRENT_PHASE_T = "off_grid_current_phase_t"
CONF_OFF_GRID_POWER_PHASE_T = "off_grid_power_phase_t"
CONF_BATTERY_ACTIVE_CONTROL = "battery_active_control"
CONF_BATTERY_ACTIVE_ONESHOT = "battery_active_oneshot"
CONF_POWER_CONTROL = "power_control"
CONF_ACTIVE_POWER_EXPORT_LIMIT = "active_power_export_limit"
CONF_ACTIVE_POWER_IMPORT_LIMIT = "active_power_import_limit"
CONF_REACTIVE_POWER_SETTING = "reactive_power_setting"
CONF_POWER_FACTOR_SETTING = "power_factor_setting"
CONF_ACTIVE_POWER_LIMIT_SPEED = "active_power_limit_speed"
CONF_REACTIVE_POWER_RESPONSE_TIME = "reactive_power_response_time"

UPDATE_INTERVAL = "update_interval"
DEFAULT_VALUE = "default_value"
ENFORCE_DEFAULT_VALUE = "enforce_default_value"
MAX_FLAT_CHANGE = "max_flat_change"
MAX_PERCENTAGE_CHANGE = "max_percentage_change"

TYPES = {
    CONF_PV_GENERATION_TODAY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_GENERATION_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_LOAD_CONSUMPTION_TODAY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_LOAD_CONSUMPTION_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_TODAY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_DISCHARGE_TODAY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_DISCHARGE_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_TOTAL_ACTIVE_POWER_INVERTER: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_VOLTAGE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_CURRENT_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_POWER_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_VOLTAGE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_CURRENT_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_POWER_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_PV_POWER_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_VOLTAGE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),

    CONF_BATTERY_VOLTAGE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),

    CONF_BATTERY_VOLTAGE_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),

    CONF_BATTERY_VOLTAGE_4: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_4: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_4: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_4: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_4: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_4: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_4: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),


    CONF_BATTERY_VOLTAGE_5: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_5: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_5: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_5: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_5: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_5: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_5: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),

    CONF_BATTERY_VOLTAGE_6: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_6: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_6: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_6: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_6: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_6: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_6: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),

    CONF_BATTERY_VOLTAGE_7: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_7: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_7: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_7: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_7: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_7: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_7: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),

    CONF_BATTERY_VOLTAGE_8: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CURRENT_8: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_POWER_8: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_TEMPERATURE_ENV_8: sensor.sensor_schema(
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_8: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_HEALTH_8: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CHARGE_CYCLE_8: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),


    CONF_BATTERY_POWER_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_STATE_OF_CHARGE_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="30s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_DESIRED_GRID_POWER: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_MINIMUM_BATTERY_POWER: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_MAXIMUM_BATTERY_POWER: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_ENERGY_STORAGE_MODE: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_ENERGY_STORAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE, default=3): cv.int_range(0,7),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_ID: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_range(0, 255),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_ADDRESS: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_range(0, 255),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_PROTOCOL: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_range(0, 12),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_VOLTAGE_NOMINAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_float,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_VOLTAGE_OVER: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_float,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_VOLTAGE_CHARGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_float,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_VOLTAGE_LACK: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_float,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_VOLTAGE_DISCHARGE_STOP: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_float,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_CURRENT_CHARGE_LIMIT: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_float,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_CURRENT_DISCHARGE_LIMIT: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_float,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_DEPTH_OF_DISCHARGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_range(0, 100),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_END_OF_DISCHARGE: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_range(0, 100),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_CAPACITY: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.positive_not_null_int,
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_CELL_TYPE: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_range(0, 6),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_EPS_BUFFER: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(DEFAULT_VALUE): cv.int_range(0, 100),
            cv.Optional(ENFORCE_DEFAULT_VALUE, default=False): cv.boolean,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_CONF_CONTROL: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_FREQUENCY: sensor.sensor_schema(
        unit_of_measurement=UNIT_HERTZ,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_FREQUENCY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_VOLTAGE_PHASE_R: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_CURRENT_PHASE_R: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_POWER_PHASE_R: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_VOLTAGE_PHASE_S: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_CURRENT_PHASE_S: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_POWER_PHASE_S: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_VOLTAGE_PHASE_T: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_CURRENT_PHASE_T: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_GRID_POWER_PHASE_T: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_POWER_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_FREQUENCY: sensor.sensor_schema(
        unit_of_measurement=UNIT_HERTZ,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_FREQUENCY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_VOLTAGE_PHASE_R: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_CURRENT_PHASE_R: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_POWER_PHASE_R: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_VOLTAGE_PHASE_S: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_CURRENT_PHASE_S: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_POWER_PHASE_S: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_VOLTAGE_PHASE_T: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_CURRENT_PHASE_T: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_OFF_GRID_POWER_PHASE_T: sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_ACTIVE_CONTROL: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_BATTERY_ACTIVE_ONESHOT: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_POWER_CONTROL: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_ACTIVE_POWER_EXPORT_LIMIT: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_ACTIVE_POWER_IMPORT_LIMIT: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_REACTIVE_POWER_SETTING: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="30s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_POWER_FACTOR_SETTING: sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="30s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_ACTIVE_POWER_LIMIT_SPEED: sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="30s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
    CONF_REACTIVE_POWER_RESPONSE_TIME: sensor.sensor_schema(
        unit_of_measurement=UNIT_SECOND,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="30s"): cv.positive_time_period_seconds,
            cv.Optional(MAX_FLAT_CHANGE): cv.float_,
            cv.Optional(MAX_PERCENTAGE_CHANGE): cv.percentage_int,
        }
    ),
}

CONFIG_SCHEMA = SOFARSOLAR_INVERTER_COMPONENT_SCHEMA.extend({
    **{cv.Optional(type): schema for type, schema in TYPES.items()},
})


async def to_code(config):
    var = await cg.get_variable(config[CONF_SOFARSOLAR_INVERTER_ID])
    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(var, f"set_{type}_sensor")(sens))
            cg.add(getattr(var, f"set_{type}_sensor_update_interval")(conf[UPDATE_INTERVAL]))
            if DEFAULT_VALUE in conf:
                cg.add(getattr(var, f"set_{type}_sensor_default_value")(conf[DEFAULT_VALUE]))
            if ENFORCE_DEFAULT_VALUE in conf:
                cg.add(getattr(var, f"set_{type}_sensor_enforce_default_value")(conf[ENFORCE_DEFAULT_VALUE]))
            if MAX_FLAT_CHANGE in conf:
                cg.add(getattr(var, f"set_{type}_sensor_max_flat_change")(conf[MAX_FLAT_CHANGE]))
            if MAX_PERCENTAGE_CHANGE in conf:
                cg.add(getattr(var, f"set_{type}_sensor_max_percentage_change")(conf[MAX_PERCENTAGE_CHANGE]))