//
// Created by Johnathon Slightham on 2026-02-16.
//

#ifndef NEW_DEV_TOOLS_REMOTECONFIG_H
#define NEW_DEV_TOOLS_REMOTECONFIG_H

#include <cstdint>
#include <string>

#include "libcontrol.h"
#include "flatbuffers_generated/RobotModule_generated.h"

class RemoteConfig {
public:
    RemoteConfig(std::shared_ptr<RobotController> controller) : m_robot_controller(controller) {}
    bool set_module_id(uint8_t current_module_id, uint16_t new_module_id);
    bool set_module_type(uint8_t module_id, ModuleType module_type);
    bool set_wifi_ssid(uint8_t module_id, const std::string& ssid);
    bool set_wifi_password(uint8_t module_id, const std::string& password);
    bool set_communication_method(uint8_t module_id, uint8_t communication_method); // only option is 0 (wireless)
    void restart(uint8_t module_id);

private:
    std::shared_ptr<RobotController> m_robot_controller;
};

#endif //NEW_DEV_TOOLS_REMOTECONFIG_H
