#pragma once

#include "../sofarsolar_inverter.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
    namespace sofarsolar_inverter {
        class SofarSolar_Inverter;
        class SofarSolar_Inverter_Switch : public switch_::Switch, public Component {
            public:
                void set_parent(SofarSolar_Inverter *parent) { this->parent_ = parent; }
                void set_on_command(const std::string &command) { this->on_command_ = on_command; }
                void set_off_command(const std::string &command) { this->off_command_ = command; };
                void dump_config() override;

            protected:
                void write_state(bool state) override;
                std::string on_command_;
                std::string off_command_;
                Pipsolar *parent_;
        };
    }
}