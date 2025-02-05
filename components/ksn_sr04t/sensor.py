import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    STATE_CLASS_MEASUREMENT,
    UNIT_CENTIMETER,
    ICON_ARROW_EXPAND_VERTICAL,
    CONF_MODEL,
)

CODEOWNERS = ["@Mafus1"]
DEPENDENCIES = ["uart"]

ksn_sr04t_ns = cg.esphome_ns.namespace("ksn_sr04t")
Ksnsr04tComponent = ksn_sr04t_ns.class_(
    "Ksnsr04tComponent", sensor.Sensor, cg.PollingComponent, uart.UARTDevice
)
Model = ksn_sr04t_ns.enum("Model")
MODEL = {
    "ksn_sr04t": Model.KSN_SR04T,
    "aj_sr04m": Model.AJ_SR04M,
    "aj_sr04m2": Model.AJ_SR04M2
}

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        Ksnsr04tComponent,
        unit_of_measurement=UNIT_CENTIMETER,
        icon=ICON_ARROW_EXPAND_VERTICAL,
        accuracy_decimals=3,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(
        {
            cv.Optional(CONF_MODEL, default="ksn_sr04t"): cv.enum(MODEL, upper=False),
        }
    )
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "ksn_sr04t",
    baud_rate=9600,
    require_tx=True,
    require_rx=True,
    data_bits=8,
    parity=None,
    stop_bits=1,
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_model(config[CONF_MODEL]))