#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include "libcontrol.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class DeviceDetailsApp;

class DeviceListApp {
public:
    DeviceListApp();
    void run();

private:
    std::shared_ptr<RobotController> robot_controller_;
    std::vector<uint8_t> device_ids_;
    std::vector<std::string> device_list_entries_;
    int selected_device_ = 0;
    Component main_component_;

    void refreshDevices();
    void setupUI();
    void openDeviceDetails();
};
