#include "BatteryConfigWriteButton.h"

namespace esphome {
    namespace sofarsolar_inverter {

        void BatteryConfigWriteButton::press_action() { this->parent_->battery_config_write(); }

    }  // namespace sofarsolar_inverter
}  // namespace esphome