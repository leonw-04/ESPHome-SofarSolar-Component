#include "BatteryActivationButton.h"

namespace esphome {
    namespace sofarsolar_inverter {

        void BatteryActivationButton::press_action() { this->parent_->battery_activation(); }

    }  // namespace sofarsolar_inverter
}  // namespace esphome