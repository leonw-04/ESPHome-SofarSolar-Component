import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv

from .. import CONF_SOFARSOLAR_INVERTER_ID, SOFARSOLAR_INVERTER_COMPONENT_SCHEMA, sofarsolar_inverter_ns

DEPENDENCIES = ["modbus", "button"]

CONF_BATTERY_ACTIVATION_BUTTON = "battery_activation"
CONF_BATTERY_CONFIG_WRITE_BUTTON = "battery_config_write"

BatteryActivationButton = sofarsolar_inverter_ns.class_("BatteryActivationButton", button.Button)
BatteryConfigWriteButton = sofarsolar_inverter_ns.class_("BatteryConfigWriteButton", button.Button)

CONFIG_SCHEMA = SOFARSOLAR_INVERTER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_BATTERY_ACTIVATION_BUTTON): button.button_schema(
            BatteryActivationButton,
        ),
        cv.Optional(CONF_BATTERY_CONFIG_WRITE_BUTTON): button.button_schema(
            BatteryConfigWriteButton,
        ),
    }
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_SOFARSOLAR_INVERTER_ID])
    if battery_activation_conf := config.get(CONF_BATTERY_ACTIVATION_BUTTON):
        b = await button.new_button(battery_activation_conf)
        await cg.register_parented(b, config[CONF_SOFARSOLAR_INVERTER_ID])
        cg.add(paren.set_battery_activation_button(b))
    if battery_write_conf := config.get(CONF_BATTERY_CONFIG_WRITE_BUTTON):
        b = await button.new_button(battery_write_conf)
        await cg.register_parented(b, config[CONF_SOFARSOLAR_INVERTER_ID])
        cg.add(paren.set_battery_config_write_button(b))