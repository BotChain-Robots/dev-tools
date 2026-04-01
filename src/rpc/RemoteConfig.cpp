
#include "rpc/RemoteConfig.h"

bool RemoteConfig::set_module_id(uint8_t current_module_id, uint16_t new_module_id) {
    std::vector<uint8_t> vec;
    vec.resize(2);
    std::memcpy(vec.data(), &new_module_id, 2);
    m_robot_controller->remote_call(8, current_module_id, vec);
    return false;
}

bool RemoteConfig::set_module_type(uint8_t module_id, ModuleType module_type) {
    std::vector<uint8_t> vec{(uint8_t)module_type};
    const auto maybe = m_robot_controller->remote_call(9, module_id, vec);
    if (maybe) {
        return (*maybe)->at(0) > 0;
    }
    return false;
}

bool RemoteConfig::set_wifi_ssid(uint8_t module_id, const std::string& ssid) {
    std::vector<uint8_t> vec(ssid.begin(), ssid.end());
    vec.push_back('\0');
    const auto maybe = m_robot_controller->remote_call(10, module_id, vec);
    if (maybe) {
        return (*maybe)->at(0) > 0;
    }
    return false;
}

bool RemoteConfig::set_wifi_password(uint8_t module_id, const std::string& password) {
    std::vector<uint8_t> vec(password.begin(), password.end());
    vec.push_back('\0');
    const auto maybe = m_robot_controller->remote_call(11, module_id, vec);
    if (maybe) {
        return (*maybe)->at(0) > 0;
    }
    return false;
}

bool RemoteConfig::set_communication_method(uint8_t module_id, uint8_t communication_method) {
    std::vector<uint8_t> vec{communication_method};
    const auto maybe = m_robot_controller->remote_call(12, module_id, vec);
    if (maybe) {
        return (*maybe)->at(0) > 0;
    }
    return false;
}

void RemoteConfig::restart(uint8_t module_id) {
    // Will never return since the module will shutdown
    m_robot_controller->remote_call(7, module_id, {});
}
