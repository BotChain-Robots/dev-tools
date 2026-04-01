#pragma once

#include <memory>
#include <string>
#include <chrono>
#include "libcontrol.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class InformationTab {
public:
    InformationTab(std::shared_ptr<RobotController> robot_controller, uint8_t device_id);
    Component createComponent();

private:
    std::shared_ptr<RobotController> robot_controller_;
    uint8_t device_id_;

    std::string getModuleTypeString(ModuleType type);
    std::string getConnectionTypeString(Messaging::ConnectionType type);
    std::string formatTimeSince(const std::chrono::time_point<std::chrono::system_clock>& time_point);
};
