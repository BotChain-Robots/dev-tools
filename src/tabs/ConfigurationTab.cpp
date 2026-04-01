#include "tabs/ConfigurationTab.h"
#include <ftxui/dom/elements.hpp>
#include <format>
#include <thread>

ConfigurationTab::ConfigurationTab(std::shared_ptr<RobotController> robot_controller, uint8_t device_id)
    : robot_controller_(robot_controller),
      remote_config_(std::make_shared<RemoteConfig>(robot_controller)),
      device_id_(device_id) {
    // Initialize module ID input with current device ID
    module_id_input_ = std::to_string(static_cast<int>(device_id_));
}

Component ConfigurationTab::createComponent() {
    // Module ID input and button
    module_id_input_component_ = Input(&module_id_input_, "Enter new module ID (0-65535)");
    module_id_button_ = Button("Set Module ID", [this] { setModuleId(); });

    // Module Type dropdown and button
    module_type_dropdown_ = Dropdown(&module_type_options_, &selected_module_type_);
    module_type_button_ = Button("Set Module Type", [this] { setModuleType(); });

    // WiFi SSID input and button
    wifi_ssid_input_component_ = Input(&wifi_ssid_input_, "Enter WiFi SSID");
    wifi_ssid_button_ = Button("Set WiFi SSID", [this] { setWiFiSSID(); });

    // WiFi Password input and button
    InputOption password_option;
    password_option.password = true;
    wifi_password_input_component_ = Input(&wifi_password_input_, "Enter WiFi Password", password_option);
    wifi_password_button_ = Button("Set WiFi Password", [this] { setWiFiPassword(); });

    // Communication Method dropdown and button
    comm_method_dropdown_ = Dropdown(&communication_method_options_, &selected_communication_method_);
    comm_method_button_ = Button("Set Communication Method", [this] { setCommunicationMethod(); });

    // Restart button
    restart_button_ = Button("Restart Device", [this] { restartDevice(); });

    auto container = Container::Vertical({
        Container::Horizontal({module_id_input_component_, module_id_button_}),
        Container::Horizontal({module_type_dropdown_, module_type_button_}),
        Container::Horizontal({wifi_ssid_input_component_, wifi_ssid_button_}),
        Container::Horizontal({wifi_password_input_component_, wifi_password_button_}),
        Container::Horizontal({comm_method_dropdown_, comm_method_button_}),
        restart_button_
    });

    return Renderer(container, [this] {
        Elements content = {
            text("Device Configuration") | bold | hcenter,
            separator(),
            text("Module ID") | bold,
            hbox({
                module_id_input_component_->Render() | flex,
                separator(),
                module_id_button_->Render()
            }),
        };

        if (!module_id_status_.empty()) {
            bool success = module_id_status_.find("success") != std::string::npos;
            content.push_back(text(module_id_status_) | color(success ? Color::Green : Color::Red) | dim);
        }

        content.push_back(separator());
        content.push_back(text("Module Type") | bold);
        content.push_back(hbox({
            module_type_dropdown_->Render() | flex,
            separator(),
            module_type_button_->Render()
        }));

        if (!module_type_status_.empty()) {
            bool success = module_type_status_.find("success") != std::string::npos;
            content.push_back(text(module_type_status_) | color(success ? Color::Green : Color::Red) | dim);
        }

        content.push_back(separator());
        content.push_back(text("WiFi SSID") | bold);
        content.push_back(hbox({
            wifi_ssid_input_component_->Render() | flex,
            separator(),
            wifi_ssid_button_->Render()
        }));

        if (!wifi_ssid_status_.empty()) {
            bool success = wifi_ssid_status_.find("success") != std::string::npos;
            content.push_back(text(wifi_ssid_status_) | color(success ? Color::Green : Color::Red) | dim);
        }

        content.push_back(separator());
        content.push_back(text("WiFi Password") | bold);
        content.push_back(hbox({
            wifi_password_input_component_->Render() | flex,
            separator(),
            wifi_password_button_->Render()
        }));

        if (!wifi_password_status_.empty()) {
            bool success = wifi_password_status_.find("success") != std::string::npos;
            content.push_back(text(wifi_password_status_) | color(success ? Color::Green : Color::Red) | dim);
        }

        content.push_back(separator());
        content.push_back(text("Communication Method") | bold);
        content.push_back(hbox({
            comm_method_dropdown_->Render() | flex,
            separator(),
            comm_method_button_->Render()
        }));

        if (!communication_method_status_.empty()) {
            bool success = communication_method_status_.find("success") != std::string::npos;
            content.push_back(text(communication_method_status_) | color(success ? Color::Green : Color::Red) | dim);
        }

        content.push_back(separator());
        content.push_back(separator());
        content.push_back(restart_button_->Render() | hcenter);

        if (!restart_status_.empty()) {
            content.push_back(separator());
            content.push_back(text(restart_status_) | color(Color::Yellow) | bold | hcenter);
        }

        return vbox(content);
    });
}

void ConfigurationTab::setModuleId() {
    try {
        int new_id = std::stoi(module_id_input_);
        if (new_id < 0 || new_id > 65535) {
            module_id_status_ = "Error: Module ID must be between 0 and 65535";
            return;
        }

        bool success = remote_config_->set_module_id(device_id_, static_cast<uint16_t>(new_id));
        if (success) {
            module_id_status_ = "Module ID set successfully!";
        } else {
            module_id_status_ = "Failed to set module ID";
        }
    } catch (const std::exception& e) {
        module_id_status_ = std::format("Error: Invalid module ID format");
    }
}

void ConfigurationTab::setModuleType() {
    ModuleType type = static_cast<ModuleType>(selected_module_type_);
    bool success = remote_config_->set_module_type(device_id_, type);

    if (success) {
        module_type_status_ = std::format("Module type set to {} successfully!", module_type_options_[selected_module_type_]);
    } else {
        module_type_status_ = "Failed to set module type";
    }
}

void ConfigurationTab::setWiFiSSID() {
    if (wifi_ssid_input_.empty()) {
        wifi_ssid_status_ = "Error: SSID cannot be empty";
        return;
    }

    bool success = remote_config_->set_wifi_ssid(device_id_, wifi_ssid_input_);

    if (success) {
        wifi_ssid_status_ = "WiFi SSID set successfully!";
    } else {
        wifi_ssid_status_ = "Failed to set WiFi SSID";
    }
}

void ConfigurationTab::setWiFiPassword() {
    if (wifi_password_input_.empty()) {
        wifi_password_status_ = "Error: Password cannot be empty";
        return;
    }

    bool success = remote_config_->set_wifi_password(device_id_, wifi_password_input_);

    if (success) {
        wifi_password_status_ = "WiFi password set successfully!";
        // Clear password input for security
        wifi_password_input_.clear();
    } else {
        wifi_password_status_ = "Failed to set WiFi password";
    }
}

void ConfigurationTab::setCommunicationMethod() {
    uint8_t method = static_cast<uint8_t>(selected_communication_method_);
    bool success = remote_config_->set_communication_method(device_id_, method);

    if (success) {
        communication_method_status_ = "Communication method set successfully!";
    } else {
        communication_method_status_ = "Failed to set communication method";
    }
}

void ConfigurationTab::restartDevice() {
    restart_status_ = "Restarting device...";

    // Restart the device in a separate thread
    std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        remote_config_->restart(device_id_);
        restart_status_ = "Device restart command sent.";
    }).detach();
}

std::string ConfigurationTab::getModuleTypeString(ModuleType type) {
    int type_value = static_cast<int>(type);
    if (type_value >= 0 && type_value < static_cast<int>(module_type_options_.size())) {
        return module_type_options_[type_value];
    }
    return std::format("Unknown Type {}", type_value);
}
