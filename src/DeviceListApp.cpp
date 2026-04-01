#include "DeviceListApp.h"
#include "DeviceDetailsApp.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <format>

DeviceListApp::DeviceListApp() : robot_controller_(std::make_shared<RobotController>()) {
    refreshDevices();
    setupUI();
}

void DeviceListApp::run() {
    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(main_component_);
}

void DeviceListApp::refreshDevices() {
    // Find connected modules (returns std::unordered_set<uint8_t>)
    auto module_list = robot_controller_->getModuleList();

    // Convert unordered_set to vector
    device_ids_.clear();
    for (const auto module : module_list) {
        device_ids_.emplace_back(module.id);
    }

    // Convert to display strings
    device_list_entries_.clear();
    for (const auto& device_id : device_ids_) {
        device_list_entries_.push_back(std::format("Device ID: {}", static_cast<int>(device_id)));
    }

    // If no devices found, show a message
    if (device_list_entries_.empty()) {
        device_list_entries_.push_back("No devices found");
    }

    // Reset selection if out of bounds
    if (selected_device_ >= static_cast<int>(device_list_entries_.size())) {
        selected_device_ = 0;
    }
}

void DeviceListApp::setupUI() {
    // Create the device list component with Enter key handling
    auto device_list = Menu(&device_list_entries_, &selected_device_);

    // Override the default behavior to handle Enter key
    device_list = CatchEvent(device_list, [this](Event event) {
        if (event == Event::Return) {
            openDeviceDetails();
            return true;
        }
        return false;
    });

    // Create refresh button
    auto refresh_button = Button("Refresh", [this] {
        refreshDevices();
    });

    // Layout components
    main_component_ = Container::Vertical({
        device_list,
        refresh_button
    });

    // Add renderer to show the UI
    main_component_ = Renderer(main_component_, [=, this] {
        return vbox({
            text("DevTools") | bold | hcenter,
            separator(),
            text(std::format("Found {} devices", device_ids_.size())) | hcenter,
            separator(),
            device_list->Render() | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 15),
            separator(),
            refresh_button->Render() | hcenter,
            separator(),
            text("Use ↑/↓ to navigate, Enter to view details, Ctrl+C to quit") | dim | hcenter
        }) | border;
    });
}

void DeviceListApp::openDeviceDetails() {
    if (device_ids_.empty() || selected_device_ >= static_cast<int>(device_ids_.size())) {
        return;
    }

    // Get the selected device ID
    uint8_t selected_device_id = device_ids_[selected_device_];

    // Create and run the device details app
    DeviceDetailsApp details_app(robot_controller_, selected_device_id);
    details_app.run();

    // When we return from device details, refresh the device list
    refreshDevices();
}
