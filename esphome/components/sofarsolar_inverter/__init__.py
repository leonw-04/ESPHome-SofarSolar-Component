import esphome.codegen as cg
from esphome.components import sensor, modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["modbus"]
AUTO_LOAD = ["binary_sensor", "text_sensor", "sensor", "switch", "output"]
MULTI_CONF = True

CONF_MODEL = "model"
CONF_MODBUS_ADDRESS = "modbus_address"
CONF_ZERO_EXPORT = "zero_export"
CONF_POWER_ID = "power_id"

CONF_SOFARSOLAR_INVERTER_ID = "sofarsolar_inverter_id"

sofarsolar_inverter_ns = cg.esphome_ns.namespace("sofarsolar_inverter")
SofarSolar_Inverter = sofarsolar_inverter_ns.class_("SofarSolar_Inverter", cg.Component, modbus.ModbusDevice)

SOFARSOLAR_INVERTER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_SOFARSOLAR_INVERTER_ID): cv.use_id(CONF_SOFARSOLAR_INVERTER_ID),
    }
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SofarSolar_Inverter),
    cv.Required(CONF_MODEL): cv.string,
    cv.Optional(CONF_MODBUS_ADDRESS, default=1): cv.int_range(0, 255),
    cv.Optional(CONF_ZERO_EXPORT, default=False): cv.boolean,
    cv.Optional(CONF_POWER_ID): cv.use_id(sensor.Sensor),
}).extend(modbus.modbus_device_schema(0x01))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await modbus.register_modbus_device(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))
    cg.add(var.set_modbus_address(config[CONF_MODBUS_ADDRESS]))
    cg.add(var.set_zero_export(config[CONF_ZERO_EXPORT]))

    if bar := config.get(CONF_POWER_ID):
        power_sensor = await cg.get_variable(config[CONF_POWER_ID])
        cg.add(var.set_power_id(power_sensor))