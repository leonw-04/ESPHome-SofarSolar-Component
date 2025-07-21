import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv

from .. import CONF_SOFARSOLAR_INVERTER_ID, SOFARSOLAR_INVERTER_COMPONENT_SCHEMA

DEPENDENCIES = ["modbus"]

CONF_OPERATIONAL_STATUS = "operational_status"
CONF_SERIAL_NUMBER = "serial_number"

TYPES = {
    CONF_OPERATIONAL_STATUS,
    CONF_SERIAL_NUMBER,
}

CONFIG_SCHEMA = SOFARSOLAR_INVERTER_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): text_sensor.text_sensor_schema() for type in TYPES}
)

async def to_code(config):
    var = await cg.get_variable(config[CONF_SOFARSOLAR_INVERTER_ID])

    for type in TYPES:
        if type in config:
            sens = await text_sensor.new_text_sensor(config[type])
            cg.add(getattr(var, f"set_{type}")(sens))