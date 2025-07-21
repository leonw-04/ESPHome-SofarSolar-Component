#include "sofarsolar_inverter_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
    namespace sofarsolar_inverter {

        static const char *const TAG = "sofarsolar_inverter.switch";

        void SofarSolar_Inverter_Switch::dump_config() {
            ESP_LOGCONFIG(TAG, "SofarSolar_Inverter Switch");
            ESP_LOGCONFIG(TAG, "  on_command: '%s'", this->on_command_.c_str());
            ESP_LOGCONFIG(TAG, "  off_command: '%s'", this->off_command_.c_str());
        }

        void SofarSolar_Inverter_Switch::write_state(bool state) {
            if (state) {
                ESP_LOGD(TAG, "Turning ON: %s", this->on_command_.c_str());
            } else {
                ESP_LOGD(TAG, "Turning OFF: %s", this->off_command_.c_str());
            }
            if (this->parent_ == nullptr) {
                ESP_LOGE(TAG, "Parent SofarSolar_Inverter is not set!");
                return;
            } else
        }
    }
}