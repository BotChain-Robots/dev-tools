#include "DeviceDetailsApp.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <format>

DeviceDetailsApp::DeviceDetailsApp(std::shared_ptr<RobotController> robot_controller, uint8_t device_id)
    : robot_controller_(robot_controller),
      remote_debugging_(std::make_shared<RemoteDebugging>(robot_controller)),
      device_id_(device_id) {
    // Create tab instances
    information_tab_ = std::make_unique<InformationTab>(robot_controller_, device_id_);
    logging_tab_ = std::make_unique<LoggingTab>(remote_debugging_, device_id_);
    firmware_update_tab_ = std::make_unique<FirmwareUpdateTab>(robot_controller_, device_id_);
    task_manager_tab_ = std::make_unique<TaskManagerTab>(remote_debugging_, device_id_);
    configuration_tab_ = std::make_unique<ConfigurationTab>(robot_controller_, device_id_);
    coredump_tab_ = std::make_unique<CoredumpTab>(remote_debugging_, device_id_);

    setupUI();
}

void DeviceDetailsApp::run() {
    auto screen = ScreenInteractive::Fullscreen();
    screen.Loop(main_component_);
}

void DeviceDetailsApp::setupUI() {
    // Create tab toggle
    auto tab_toggle = Toggle(&tab_names_, &selected_tab_);

    // Create back button
    auto back_button = Button("← Back to Device List", [&] {
        // Exit the details view and return to device list
        auto screen = ScreenInteractive::Active();
        if (screen) {
            screen->Exit();
        }
    });

    // Create tab container that shows different content based on selected tab
    auto tab_container = Container::Tab({
        information_tab_->createComponent(),
        logging_tab_->createComponent(),
        firmware_update_tab_->createComponent(),
        task_manager_tab_->createComponent(),
        configuration_tab_->createComponent(),
        coredump_tab_->createComponent()
    }, &selected_tab_);

    // Main layout
    main_component_ = Container::Vertical({
        Container::Horizontal({back_button}),
        tab_toggle,
        tab_container
    });

    // Renderer
    main_component_ = Renderer(main_component_, [=, this] {
        return vbox({
            hbox({
                back_button->Render(),
                filler(),
                text(std::format("Device ID: {}", static_cast<int>(device_id_))) | bold
            }),
            separator(),
            tab_toggle->Render() | hcenter,
            separator(),
            tab_container->Render() | flex
        }) | border;
    });
}

