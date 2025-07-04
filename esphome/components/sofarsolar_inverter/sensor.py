import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import uart, sensor
from esphome.const import CONF_ID, DEVICE_CLASS_POWER, DEVICE_CLASS_ENERGY, DEVICE_CLASS_ENERGY_STORAGE, \
    DEVICE_CLASS_VOLTAGE, DEVICE_CLASS_CURRENT

from esphome.const import (
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_ENERGY_STORAGE,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_BATTERY_CHARGING,
    DEVICE_CLASS_FREQUENCY,
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
    UNIT_EMPTY
)

DEPENDENCIES = ["uart"]

CONF_MODEL = "model"
CONF_MODBUS_ADDRESS = "address"
CONF_UART_ID = "uart_id"
CONF_ZERO_EXPORT = "zero_export"
CONF_POWER_ID = "power_id"

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
UPDATE_INTERVAL = "update_interval"


sofarsolar_inverter_ns = cg.esphome_ns.namespace("sofarsolar_inverter")
SofarSolar_Inverter = sofarsolar_inverter_ns.class_("SofarSolar_Inverter", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SofarSolar_Inverter),
    cv.Required(CONF_MODEL): cv.string,
    cv.Optional(CONF_MODBUS_ADDRESS, default=1): cv.int_range(0, 255),
    cv.Optional(CONF_ZERO_EXPORT, default=False): cv.boolean,
    cv.Optional(CONF_POWER_ID): cv.entity_id,

    cv.Optional(CONF_PV_GENERATION_TODAY): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_GENERATION_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_LOAD_CONSUMPTION_TODAY): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_LOAD_CONSUMPTION_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CHARGE_TODAY): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CHARGE_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_DISCHARGE_TODAY): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="60s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_DISCHARGE_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="120s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_VOLTAGE_1): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_CURRENT_1): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_POWER_1): sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_VOLTAGE_2): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_CURRENT_2): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_POWER_2): sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_PV_POWER_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_POWER_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="10s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_STATE_OF_CHARGE_TOTAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_BATTERY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="30s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_DESIRED_GRID_POWER): sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_MINIMUM_BATTERY_POWER): sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_MAXIMUM_BATTERY_POWER): sensor.sensor_schema(
        unit_of_measurement=UNIT_WATT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="1s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_ENERGY_STORAGE_MODE): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_ID): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_ADDRESS): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_PROTOCOL): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_VOLTAGE_NOMINAL): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_VOLTAGE_OVER): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_VOLTAGE_CHARGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_VOLTAGE_LACK): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_VOLTAGE_DISCHARGE_STOP): sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_CURRENT_CHARGE_LIMIT): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_CURRENT_DISCHARGE_LIMIT): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_DEPTH_OF_DISCHARGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_END_OF_DISCHARGE): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_CAPACITY): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_CELL_TYPE): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_EPS_BUFFER): sensor.sensor_schema(
        unit_of_measurement=UNIT_PERCENT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
    cv.Optional(CONF_BATTERY_CONF_CONTROL): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    ).extend(
        {
            cv.Optional(UPDATE_INTERVAL, default="300s"): cv.positive_time_period_seconds,
        }
    ),
}).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))
    cg.add(var.set_modbus_address(config[CONF_MODBUS_ADDRESS]))
    cg.add(var.set_zero_export(config[CONF_ZERO_EXPORT]))
    if bar := config.get(CONF_POWER_ID):
        cg.add(var.set_power_id(CONF_POWER_ID))


    if pv_generation_today_config := config.get(CONF_PV_GENERATION_TODAY):
        sens = await sensor.new_sensor(pv_generation_today_config)
        cg.add(var.set_pv_generation_today_sensor(sens))
        cg.add(var.set_pv_generation_today_sensor_update_interval(pv_generation_today_config[UPDATE_INTERVAL]))

    if pv_generation_total_config := config.get(CONF_PV_GENERATION_TOTAL):
        sens = await sensor.new_sensor(pv_generation_total_config)
        cg.add(var.set_pv_generation_total_sensor(sens))
        cg.add(var.set_pv_generation_total_sensor_update_interval(pv_generation_total_config[UPDATE_INTERVAL]))

    if load_consumption_today_config := config.get(CONF_LOAD_CONSUMPTION_TODAY):
        sens = await sensor.new_sensor(load_consumption_today_config)
        cg.add(var.set_load_consumption_today_sensor(sens))
        cg.add(var.set_load_consumption_today_sensor_update_interval(load_consumption_today_config[UPDATE_INTERVAL]))

    if load_consumption_total_config := config.get(CONF_LOAD_CONSUMPTION_TOTAL):
        sens = await sensor.new_sensor(load_consumption_total_config)
        cg.add(var.set_load_consumption_total_sensor(sens))
        cg.add(var.set_load_consumption_total_sensor_update_interval(load_consumption_total_config[UPDATE_INTERVAL]))

    if battery_charge_today_config := config.get(CONF_BATTERY_CHARGE_TODAY):
        sens = await sensor.new_sensor(battery_charge_today_config)
        cg.add(var.set_battery_charge_today_sensor(sens))
        cg.add(var.set_battery_charge_today_sensor_update_interval(battery_charge_today_config[UPDATE_INTERVAL]))

    if battery_charge_total_config := config.get(CONF_BATTERY_CHARGE_TOTAL):
        sens = await sensor.new_sensor(battery_charge_total_config)
        cg.add(var.set_battery_charge_total_sensor(sens))
        cg.add(var.set_battery_charge_total_sensor_update_interval(battery_charge_total_config[UPDATE_INTERVAL]))

    if battery_discharge_today_config := config.get(CONF_BATTERY_DISCHARGE_TODAY):
        sens = await sensor.new_sensor(battery_discharge_today_config)
        cg.add(var.set_battery_discharge_today_sensor(sens))
        cg.add(var.set_battery_discharge_today_sensor_update_interval(battery_discharge_today_config[UPDATE_INTERVAL]))

    if battery_discharge_total_config := config.get(CONF_BATTERY_DISCHARGE_TOTAL):
        sens = await sensor.new_sensor(battery_discharge_total_config)
        cg.add(var.set_battery_discharge_total_sensor(sens))
        cg.add(var.set_battery_discharge_total_sensor_update_interval(battery_discharge_total_config[UPDATE_INTERVAL]))

    if pv_voltage_1_config := config.get(CONF_PV_VOLTAGE_1):
        sens = await sensor.new_sensor(pv_voltage_1_config)
        cg.add(var.set_pv_voltage_1_sensor(sens))
        cg.add(var.set_pv_voltage_1_sensor_update_interval(pv_voltage_1_config[UPDATE_INTERVAL]))

    if pv_current_1_config := config.get(CONF_PV_CURRENT_1):
        sens = await sensor.new_sensor(pv_current_1_config)
        cg.add(var.set_pv_current_1_sensor(sens))
        cg.add(var.set_pv_current_1_sensor_update_interval(pv_current_1_config[UPDATE_INTERVAL]))

    if pv_power_1_config := config.get(CONF_PV_POWER_1):
        sens = await sensor.new_sensor(pv_power_1_config)
        cg.add(var.set_pv_power_1_sensor(sens))
        cg.add(var.set_pv_power_1_sensor_update_interval(pv_power_1_config[UPDATE_INTERVAL]))

    if pv_voltage_2_config := config.get(CONF_PV_VOLTAGE_2):
        sens = await sensor.new_sensor(pv_voltage_2_config)
        cg.add(var.set_pv_voltage_2_sensor(sens))
        cg.add(var.set_pv_voltage_2_sensor_update_interval(pv_voltage_2_config[UPDATE_INTERVAL]))

    if pv_current_2_config := config.get(CONF_PV_CURRENT_2):
        sens = await sensor.new_sensor(pv_current_2_config)
        cg.add(var.set_pv_current_2_sensor(sens))
        cg.add(var.set_pv_current_2_sensor_update_interval(pv_current_2_config[UPDATE_INTERVAL]))

    if pv_power_2_config := config.get(CONF_PV_POWER_2):
        sens = await sensor.new_sensor(pv_power_2_config)
        cg.add(var.set_pv_power_2_sensor(sens))
        cg.add(var.set_pv_power_2_sensor_update_interval(pv_power_2_config[UPDATE_INTERVAL]))

    if pv_power_total_config := config.get(CONF_PV_POWER_TOTAL):
        sens = await sensor.new_sensor(pv_power_total_config)
        cg.add(var.set_pv_power_total_sensor(sens))
        cg.add(var.set_pv_power_total_sensor_update_interval(pv_power_total_config[UPDATE_INTERVAL]))

    if battery_power_total_config := config.get(CONF_BATTERY_POWER_TOTAL):
        sens = await sensor.new_sensor(battery_power_total_config)
        cg.add(var.set_battery_power_total_sensor(sens))
        cg.add(var.set_battery_power_total_sensor_update_interval(battery_power_total_config[UPDATE_INTERVAL]))

    if battery_state_of_charge_total_config := config.get(CONF_BATTERY_STATE_OF_CHARGE_TOTAL):
        sens = await sensor.new_sensor(battery_state_of_charge_total_config)
        cg.add(var.set_battery_state_of_charge_total_sensor(sens))
        cg.add(var.set_battery_state_of_charge_total_sensor_update_interval(battery_state_of_charge_total_config[UPDATE_INTERVAL]))

    if desired_grid_power_config := config.get(CONF_DESIRED_GRID_POWER):
        sens = await sensor.new_sensor(desired_grid_power_config)
        cg.add(var.set_desired_grid_power_sensor(sens))
        cg.add(var.set_desired_grid_power_sensor_update_interval(desired_grid_power_config[UPDATE_INTERVAL]))

    if minimum_battery_power_config := config.get(CONF_MINIMUM_BATTERY_POWER):
        sens = await sensor.new_sensor(minimum_battery_power_config)
        cg.add(var.set_minimum_battery_power_sensor(sens))
        cg.add(var.set_minimum_battery_power_sensor_update_interval(minimum_battery_power_config[UPDATE_INTERVAL]))

    if maximum_battery_power_config := config.get(CONF_MAXIMUM_BATTERY_POWER):
        sens = await sensor.new_sensor(maximum_battery_power_config)
        cg.add(var.set_maximum_battery_power_sensor(sens))
        cg.add(var.set_maximum_battery_power_sensor_update_interval(maximum_battery_power_config[UPDATE_INTERVAL]))

    if energy_storage_mode_config := config.get(CONF_ENERGY_STORAGE_MODE):
        sens = await sensor.new_sensor(energy_storage_mode_config)
        cg.add(var.set_energy_storage_mode_sensor(sens))
        cg.add(var.set_energy_storage_mode_sensor_update_interval(energy_storage_mode_config[UPDATE_INTERVAL]))

    if battery_conf_id_config := config.get(CONF_BATTERY_CONF_ID):
        sens = await sensor.new_sensor(battery_conf_id_config)
        cg.add(var.set_battery_conf_id_sensor(sens))
        cg.add(var.set_battery_conf_id_sensor_update_interval(battery_conf_id_config[UPDATE_INTERVAL]))

    if battery_conf_address_config := config.get(CONF_BATTERY_CONF_ADDRESS):
        sens = await sensor.new_sensor(battery_conf_address_config)
        cg.add(var.set_battery_conf_address_sensor(sens))
        cg.add(var.set_battery_conf_address_sensor_update_interval(battery_conf_address_config[UPDATE_INTERVAL]))

    if battery_conf_protocol_config := config.get(CONF_BATTERY_CONF_PROTOCOL):
        sens = await sensor.new_sensor(battery_conf_protocol_config)
        cg.add(var.set_battery_conf_protocol_sensor(sens))
        cg.add(var.set_battery_conf_protocol_sensor_update_interval(battery_conf_protocol_config[UPDATE_INTERVAL]))

    if battery_conf_voltage_nominal_config := config.get(CONF_BATTERY_CONF_VOLTAGE_NOMINAL):
        sens = await sensor.new_sensor(battery_conf_voltage_nominal_config)
        cg.add(var.set_battery_conf_voltage_nominal_sensor(sens))
        cg.add(var.set_battery_conf_voltage_nominal_sensor_update_interval(battery_conf_voltage_nominal_config[UPDATE_INTERVAL]))

    if battery_conf_voltage_over_config := config.get(CONF_BATTERY_CONF_VOLTAGE_OVER):
        sens = await sensor.new_sensor(battery_conf_voltage_over_config)
        cg.add(var.set_battery_conf_voltage_over_sensor(sens))
        cg.add(var.set_battery_conf_voltage_over_sensor_update_interval(battery_conf_voltage_over_config[UPDATE_INTERVAL]))

    if battery_conf_voltage_charge_config := config.get(CONF_BATTERY_CONF_VOLTAGE_CHARGE):
        sens = await sensor.new_sensor(battery_conf_voltage_charge_config)
        cg.add(var.set_battery_conf_voltage_charge_sensor(sens))
        cg.add(var.set_battery_conf_voltage_charge_sensor_update_interval(battery_conf_voltage_charge_config[UPDATE_INTERVAL]))

    if battery_conf_voltage_lack_config := config.get(CONF_BATTERY_CONF_VOLTAGE_LACK):
        sens = await sensor.new_sensor(battery_conf_voltage_lack_config)
        cg.add(var.set_battery_conf_voltage_lack_sensor(sens))
        cg.add(var.set_battery_conf_voltage_lack_sensor_update_interval(battery_conf_voltage_lack_config[UPDATE_INTERVAL]))

    if battery_conf_voltage_discharge_stop_config := config.get(CONF_BATTERY_CONF_VOLTAGE_DISCHARGE_STOP):
        sens = await sensor.new_sensor(battery_conf_voltage_discharge_stop_config)
        cg.add(var.set_battery_conf_voltage_discharge_stop_sensor(sens))
        cg.add(var.set_battery_conf_voltage_discharge_stop_sensor_update_interval(battery_conf_voltage_discharge_stop_config[UPDATE_INTERVAL]))

    if battery_conf_current_charge_limit_config := config.get(CONF_BATTERY_CONF_CURRENT_CHARGE_LIMIT):
        sens = await sensor.new_sensor(battery_conf_current_charge_limit_config)
        cg.add(var.set_battery_conf_current_charge_limit_sensor(sens))
        cg.add(var.set_battery_conf_current_charge_limit_sensor_update_interval(battery_conf_current_charge_limit_config[UPDATE_INTERVAL]))

    if battery_conf_current_discharge_limit_config := config.get(CONF_BATTERY_CONF_CURRENT_DISCHARGE_LIMIT):
        sens = await sensor.new_sensor(battery_conf_current_discharge_limit_config)
        cg.add(var.set_battery_conf_current_discharge_limit_sensor(sens))
        cg.add(var.set_battery_conf_current_discharge_limit_sensor_update_interval(battery_conf_current_discharge_limit_config[UPDATE_INTERVAL]))

    if battery_conf_depth_of_discharge_config := config.get(CONF_BATTERY_CONF_DEPTH_OF_DISCHARGE):
        sens = await sensor.new_sensor(battery_conf_depth_of_discharge_config)
        cg.add(var.set_battery_conf_depth_of_discharge_sensor(sens))
        cg.add(var.set_battery_conf_depth_of_discharge_sensor_update_interval(battery_conf_depth_of_discharge_config[UPDATE_INTERVAL]))

    if battery_conf_end_of_discharge_config := config.get(CONF_BATTERY_CONF_END_OF_DISCHARGE):
        sens = await sensor.new_sensor(battery_conf_end_of_discharge_config)
        cg.add(var.set_battery_conf_end_of_discharge_sensor(sens))
        cg.add(var.set_battery_conf_end_of_discharge_sensor_update_interval(battery_conf_end_of_discharge_config[UPDATE_INTERVAL]))

    if battery_conf_capacity_config := config.get(CONF_BATTERY_CONF_CAPACITY):
        sens = await sensor.new_sensor(battery_conf_capacity_config)
        cg.add(var.set_battery_conf_capacity_sensor(sens))
        cg.add(var.set_battery_conf_capacity_sensor_update_interval(battery_conf_capacity_config[UPDATE_INTERVAL]))

    if battery_conf_cell_type_config := config.get(CONF_BATTERY_CONF_CELL_TYPE):
        sens = await sensor.new_sensor(battery_conf_cell_type_config)
        cg.add(var.set_battery_conf_cell_type_sensor(sens))
        cg.add(var.set_battery_conf_cell_type_sensor_update_interval(battery_conf_cell_type_config[UPDATE_INTERVAL]))

    if battery_conf_eps_buffer_config := config.get(CONF_BATTERY_CONF_EPS_BUFFER):
        sens = await sensor.new_sensor(battery_conf_eps_buffer_config)
        cg.add(var.set_battery_conf_eps_buffer_sensor(sens))
        cg.add(var.set_battery_conf_eps_buffer_sensor_update_interval(battery_conf_eps_buffer_config[UPDATE_INTERVAL]))

    if battery_conf_control_config := config.get(CONF_BATTERY_CONF_CONTROL):
        sens = await sensor.new_sensor(battery_conf_control_config)
        cg.add(var.set_battery_conf_control_sensor(sens))
        cg.add(var.set_battery_conf_control_sensor_update_interval(battery_conf_control_config[UPDATE_INTERVAL]))