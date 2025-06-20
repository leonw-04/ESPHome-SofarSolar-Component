import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import uart
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

CONF_MODEL = "model"
CONF_MODBUS_ADDRESS = "address"
CONF_UART_ID = "uart_id"
CONF_ZERO_EXPORT = "zero_export"
CONF_POWER_ID = "power_id"

sofarsolar_inverter_ns = cg.esphome_ns.namespace("sofarsolar_inverter")
SofarSolar_Inverter = sofarsolar_inverter_ns.class_("SofarSolar_Inverter", cg.Component, uart.UARTDevice)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(SofarSolar_Inverter),
    cv.Required(CONF_MODEL): cv.string,
    cv.Optional(CONF_MODBUS_ADDRESS, default=1): cv.int_range(0, 255),
    cv.Required(CONF_UART_ID): cv.string,
    cv.Optional(CONF_ZERO_EXPORT, default=False): cv.boolean,
    cv.Optional(CONF_POWER_ID): cv.string,
}).extend(uart.UART_DEVICE_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))
    cg.add(var.set_modbus_address(config[CONF_MODBUS_ADDRESS]))
    cg.add(var.set_uart_id(config[CONF_UART_ID]))
    cg.add(var.set_zero_export(config[CONF_ZERO_EXPORT]))
    if bar := config.get(CONF_POWER_ID):
        cg.add(var.set_power_id(CONF_POWER_ID))