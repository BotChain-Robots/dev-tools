#pragma once

#include <memory>
#include <string>
#include "libcontrol.h"
#include "rpc/RemoteConfig.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class ConfigurationTab {
public:
    ConfigurationTab(std::shared_ptr<RobotController> robot_controller, uint8_t device_id);
    Component createComponent();

private:
    std::shared_ptr<RobotController> robot_controller_;
    std::shared_ptr<RemoteConfig> remote_config_;
    uint8_t device_id_;

    // Input fields
    std::string module_id_input_;
    int selected_module_type_ = 0;
    std::string wifi_ssid_input_;
    std::string wifi_password_input_;
    int selected_communication_method_ = 0;

    // Status messages
    std::string module_id_status_;
    std::string module_type_status_;
    std::string wifi_ssid_status_;
    std::string wifi_password_status_;
    std::string communication_method_status_;
    std::string restart_status_;

    // Component members
    Component module_id_input_component_;
    Component module_id_button_;
    Component module_type_dropdown_;
    Component module_type_button_;
    Component wifi_ssid_input_component_;
    Component wifi_ssid_button_;
    Component wifi_password_input_component_;
    Component wifi_password_button_;
    Component comm_method_dropdown_;
    Component comm_method_button_;
    Component restart_button_;

    // Module type options
    std::vector<std::string> module_type_options_ = {
        "SPLITTER",
        "SERVO_1",
        "DC_MOTOR",
        "BATTERY",
        "SERVO_2",
        "DISPLAY",
        "GRIPPER",
        "SPEAKER",
        "IMU",
        "DISTANCE_SENSOR",
        "SPLITTER_2",
        "SPLITTER_3",
        "SPLITTER_4"
    };

    std::vector<std::string> communication_method_options_ = {
        "Wireless"
    };

    // Helper methods
    void setModuleId();
    void setModuleType();
    void setWiFiSSID();
    void setWiFiPassword();
    void setCommunicationMethod();
    void restartDevice();
    std::string getModuleTypeString(ModuleType type);
};
