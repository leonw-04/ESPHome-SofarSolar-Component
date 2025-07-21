import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv

from .. import CONF_SOFARSOLAR_INVERTER_ID, SOFARSOLAR_INVERTER_COMPONENT_SCHEMA, sofarsolar_inverter_ns

DEPENDENCIES = ["modbus"]

CONF_BATTERY_CHARGE_ONLY = "battery_charge_only"
CONF_BATTERY_DISCHARGE_ONLY = "battery_discharge_only"

TYPES = {
    CONF_BATTERY_CHARGE_ONLY,
    CONF_BATTERY_DISCHARGE_ONLY,
}

SofarSolar_Inverter_Switch = sofarsolar_inverter_ns.class_("SofarSolar_Inverter_Switch", switch.Switch, cg.Component)

SOFARSOLAR_INVERTER_SWITCH_SCHEMA = switch.switch_schema(SofarSolar_Inverter_Switch).extend({cv.COMPONENT_SCHEMA})

CONFIG_SCHEMA = SOFARSOLAR_INVERTER_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): SOFARSOLAR_INVERTER_SWITCH_SCHEMA for type in TYPES}
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_SOFARSOLAR_INVERTER_ID])

    for type, (on, off) in TYPES.items():
        if type in config:
            conf = config[type]
            var = await switch.new_switch(conf)
            await cg.register_component(var, conf)
            cg.add(getattr(paren, f"set_{type}_switch")(var))
            cg.add(var.set_parent(paren))
            cg.add(var.set_on_command(on))
            if off is not None:
                cg.add(var.set_off_command(off))