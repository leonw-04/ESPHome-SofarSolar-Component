# ESPHome-SofarSolar-Component
This is an external Component for the SolarSofar Inverter. Especially the HYD-6000 EP. It used the G3 Protocol from SofarSolar. It supports the following features:
- Read the current power production
- Read the current power consumption
- Read the current battery status and configuration

Communication is done via Modbus RTU. UART over R485. The component is designed to be used with ESPHome as External Component.

# Example Configuration
Tested on an ESP32 S3 with a SofarSolar HYD-6000 EP Inverter. The configuration below is an example of how to set up the component in your ESPHome configuration file.

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/leonw-04/ESPHome-SofarSolar-Component
    refresh: 0s

uart:
  id: mod_bus
  tx_pin:
    number: GPIO16
  rx_pin:
    number: GPIO15
  baud_rate: 9600
  stop_bits: 1
  debug:
    direction: BOTH
    dummy_receiver: true


modbus:
  id: modbus_id
  uart_id: mod_bus
  disable_crc: False
  role: client
  send_wait_time: 100ms

sofarsolar_inverter:
  id: pv
  modbus_id: modbus_id
  model: "hyd6000-ep"
  address: 01
  zero_export: True
  power_id: total_active_power_house

sensor:
  - platform: homeassistant
    id: total_active_power_house
    entity_id: sensor.stromzaehler_momentane_leistung

  - platform: sofarsolar_inverter
    sofarsolar_inverter_id: pv
    pv_generation_today:
      name: "PV Generation Today"
      id: pv_generation_today
      max_flat_change: 0.1
    pv_generation_total:
      name: "PV Generation Total"
      id: pv_generation_total
      max_flat_change: 0.1
    load_consumption_today:
      name: "Load Consumption Today"
      id: load_consumption_today
      max_flat_change: 0.1
    load_consumption_total:
      name: "Load Consumption Total"
      id: load_consumption_total
      max_flat_change: 0.1
    battery_charge_today:
      name: "Battery Charge Today"
      id: battery_charge_today
      max_flat_change: 0.1
    battery_charge_total:
      name: "Battery Charge Total"
      id: battery_charge_total
      max_flat_change: 0.1
    battery_discharge_today:
      name: "Battery Discharge Today"
      id: battery_discharge_today
      max_flat_change: 0.1
    battery_discharge_total:
      name: "Battery Discharge Total"
      id: battery_discharge_total
      max_flat_change: 0.1
    total_active_power_inverter:
      name: "Total Active Power Inverter"
      id: total_active_power_inverter
      filters:
        - clamp:
            min_value: -10000
            max_value: 10000
            ignore_out_of_range: true
    pv_voltage_1:
      name: "PV Voltage 1"
      id: pv_voltage_1
      filters:
        - clamp:
            min_value: -10
            max_value: 1000
            ignore_out_of_range: true
    pv_current_1:
      name: "PV Current 1"
      id: pv_current_1
      filters:
        - clamp:
            min_value: -1
            max_value: 100
            ignore_out_of_range: true
    pv_power_1:
      name: "PV Power 1"
      id: pv_power_1
      filters:
        - clamp:
            min_value: -100
            max_value: 10000
            ignore_out_of_range: true
    pv_voltage_2:
      name: "PV Voltage 2"
      id: pv_voltage_2
      filters:
        - clamp:
            min_value: -10
            max_value: 1000
            ignore_out_of_range: true
    pv_current_2:
      name: "PV Current 2"
      id: pv_current_2
      filters:
        - clamp:
            min_value: -1
            max_value: 100
            ignore_out_of_range: true
    pv_power_2:
      name: "PV Power 2"
      id: pv_power_2
      filters:
        - clamp:
            min_value: -100
            max_value: 10000
            ignore_out_of_range: true
    pv_power_total:
      name: "PV Power Total"
      id: pv_power_total
      filters:
        - clamp:
            min_value: -100
            max_value: 10000
            ignore_out_of_range: true
    battery_power_total:
      name: "Battery Power Total"
      id: battery_power_total
      filters:
        - clamp:
            min_value: -10000
            max_value: 10000
            ignore_out_of_range: true
    battery_state_of_charge_total:
      name: "Battery State of Charge"
      id: battery_state_of_charge_total
      filters:
        - clamp:
            min_value: 0
            max_value: 100
            ignore_out_of_range: true
    desired_grid_power:
      name: "Desired Grid Power"
      id: desired_grid_power
      filters:
        - clamp:
            min_value: -10000
            max_value: 10000
            ignore_out_of_range: true
    minimum_battery_power:
      name: "Minimum Batter Power"
      id: minimum_battery_power
      filters:
        - clamp:
            min_value: -10000
            max_value: 10000
            ignore_out_of_range: true
    maximum_battery_power:
      name: "Maximum Battery Power"
      id: maximum_battery_power
      filters:
        - clamp:
            min_value: -10000
            max_value: 10000
            ignore_out_of_range: true
    energy_storage_mode:
      name: "Energy Storage Mode"
      id: energy_storage_mode
      filters:
        - clamp:
            min_value: 0
            max_value: 10
            ignore_out_of_range: true
    battery_conf_id:
      name: "Battery Config ID"
      id: battery_conf_id
    battery_conf_address:
      name: "Battery Config Address"
      id: battery_conf_address
    battery_conf_protocol:
      name: "Battery Config Protocol"
      id: battery_conf_protocol
    battery_conf_voltage_nominal:
      name: "Battery Config Voltage Nominal"
      id: battery_conf_voltage_nominal
    battery_conf_voltage_over:
      name: "Battery Config Voltage Over"
      id: battery_conf_voltage_over
    battery_conf_voltage_charge:
      name: "Battery Config Voltage Charge"
      id: battery_conf_voltage_charge
    battery_conf_voltage_lack:
      name: "Battery Config Voltage Lack"
      id: battery_conf_voltage_lack
    battery_conf_voltage_discharge_stop:
      name: "Battery Config Discharge Stop"
      id: battery_conf_voltage_discharge_stop
    battery_conf_current_charge_limit:
      name: "Battery Config Current Charge Limit"
      id: battery_conf_current_charge_limit
    battery_conf_current_discharge_limit:
      name: "Battery Config Current Discharge Limit"
      id: battery_conf_current_discharge_limit
    battery_conf_depth_of_discharge:
      name: "Battery Config Depth of Discharge"
      id: battery_conf_depth_of_discharge
    battery_conf_end_of_discharge:
      name: "Battery Config End of Discharge"
      id: battery_conf_end_of_discharge
    battery_conf_capacity:
      name: "Battery Config Capacity"
      id: battery_conf_capacity
    battery_conf_cell_type:
      name: "Battery Config Cell Type"
      id: battery_conf_cell_type
    battery_conf_eps_buffer:
      name: "Battery Config EPS Buffer"
      id: battery_conf_eps_buffer
    battery_conf_control:
      name: "Battery Config Control"
      id: battery_conf_control
    power_control:
      name: "Power Control"
      id: power_control
    active_power_export_limit:
      name: "Active Power Export Limit"
      id: active_power_export_limit
      filters:
        - clamp:
            min_value: 0
            max_value: 100
            ignore_out_of_range: true
    active_power_import_limit:
      name: "Active Power Import Limit"
      id: active_power_import_limit
      filters:
        - clamp:
            min_value: 0
            max_value: 100
            ignore_out_of_range: true
```

