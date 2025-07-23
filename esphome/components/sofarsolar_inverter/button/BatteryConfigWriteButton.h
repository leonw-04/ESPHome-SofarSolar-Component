#pragma once

#include "esphome/components/button/button.h"
#include "../sofarsolar_inverter.h"

namespace esphome {
    namespace sofarsolar_inverter {

        class BatteryConfigWriteButton : public button::Button, public Parented<SofarSolar_Inverter> {
        public:
            BatteryConfigWriteButton() = default;

        protected:
            void press_action() override;
        };

    }  // namespace sofarsolar_inverter
}  // namespace esphome