#pragma once

#include <memory>
#include <string>
#include <vector>

#include "libcontrol.h"
#include "rpc/RemoteDebugging.h"
#include "tabs/InformationTab.h"
#include "tabs/LoggingTab.h"
#include "tabs/FirmwareUpdateTab.h"
#include "tabs/TaskManagerTab.h"
#include "tabs/ConfigurationTab.h"
#include "tabs/CoredumpTab.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class DeviceDetailsApp {
public:
    DeviceDetailsApp(std::shared_ptr<RobotController> robot_controller, uint8_t device_id);
    void run();

private:
    std::shared_ptr<RobotController> robot_controller_;
    std::shared_ptr<RemoteDebugging> remote_debugging_;
    uint8_t device_id_;
    int selected_tab_ = 0;
    Component main_component_;

    // Tab instances
    std::unique_ptr<InformationTab> information_tab_;
    std::unique_ptr<LoggingTab> logging_tab_;
    std::unique_ptr<FirmwareUpdateTab> firmware_update_tab_;
    std::unique_ptr<TaskManagerTab> task_manager_tab_;
    std::unique_ptr<ConfigurationTab> configuration_tab_;
    std::unique_ptr<CoredumpTab> coredump_tab_;

    // Tab names
    std::vector<std::string> tab_names_ = {"Information", "Logging", "Firmware Update", "TaskManager", "Configuration", "Core Dump"};

    void setupUI();
};
