#pragma once

#include <memory>
#include <string>
#include <atomic>
#include "libcontrol.h"
#include "rpc/RemoteManagement.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class FirmwareUpdateTab {
public:
    FirmwareUpdateTab(std::shared_ptr<RobotController> robot_controller, uint8_t device_id);
    Component createComponent();

private:
    std::shared_ptr<RobotController> robot_controller_;
    std::shared_ptr<RemoteManagement> remote_management_;
    uint8_t device_id_;

    // Firmware update state
    std::string firmware_file_path_;
    bool update_in_progress_ = false;
    int progress_ = 0;
    std::string update_status_message_;
    bool update_success_ = false;
};
