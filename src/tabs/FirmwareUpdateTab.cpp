#include "tabs/FirmwareUpdateTab.h"
#include <ftxui/dom/elements.hpp>
#include <format>
#include <thread>
#include <atomic>

FirmwareUpdateTab::FirmwareUpdateTab(std::shared_ptr<RobotController> robot_controller, uint8_t device_id)
    : robot_controller_(robot_controller),
      remote_management_(nullptr),
      device_id_(device_id) {
}

Component FirmwareUpdateTab::createComponent() {
    auto file_input = Input(&firmware_file_path_, "Enter firmware file path...");

    auto upload_button = Button("Upload Firmware", [&, this] {
        if (!firmware_file_path_.empty() && !update_in_progress_) {
            update_in_progress_ = true;
            progress_ = 0;
            update_status_message_ = "Initializing firmware update...";
            update_success_ = false;

            // Create RemoteManagement instance with the firmware file
            remote_management_ = std::make_shared<RemoteManagement>(
                device_id_,
                firmware_file_path_,
                robot_controller_
            );

            // Start the OTA update process in a background thread to avoid blocking UI
            std::thread([this]() {
                update_status_message_ = "Starting OTA process...";

                // Start a separate thread to monitor progress
                std::atomic<bool> ota_running{true};
                std::thread progress_monitor([this, &ota_running]() {
                    while (ota_running) {
                        if (remote_management_) {
                            double progress_percent = remote_management_->ota_progress();
                            progress_ = static_cast<int>(progress_percent * 100);
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                });

                bool success = remote_management_->perform_ota();

                ota_running = false;
                progress_monitor.join();

                if (success) {
                    progress_ = 100;
                    update_status_message_ = "Firmware update completed successfully!";
                    update_success_ = true;
                } else {
                    progress_ = 0;
                    update_status_message_ = "Firmware update failed. Please check the file and try again.";
                    update_success_ = false;
                }

                update_in_progress_ = false;
            }).detach();
        }
    });

    auto restart_button = Button("Restart Device", [&, this] {
        if (remote_management_ && !update_in_progress_) {
            update_status_message_ = "Restarting device...";
            std::thread([this]() {
                remote_management_->restart();
                update_status_message_ = "Device restart command sent.";
            }).detach();
        }
    });

    auto container = Container::Vertical({
        file_input,
        upload_button,
        restart_button
    });

    return Renderer(container, [=, this] {
        Elements content = {
            text("Firmware Update") | bold | hcenter,
            separator(),
            hbox({
                text("Firmware File: "),
                file_input->Render() | flex
            }),
            separator(),
            upload_button->Render() | hcenter,
            separator()
        };

        if (update_in_progress_) {
            content.push_back(text(update_status_message_) | bold | hcenter | color(Color::Blue));
            content.push_back(separator());
            content.push_back(gauge(static_cast<float>(progress_) / 100.0f) | color(Color::Blue));
            content.push_back(text(std::format("{}%", progress_)) | hcenter);
        } else if (!update_status_message_.empty()) {
            // Show the last status message
            auto status_color = update_success_ ? Color::Green : Color::Red;
            content.push_back(text(update_status_message_) | bold | hcenter | color(status_color));

            if (update_success_) {
                content.push_back(separator());
                content.push_back(restart_button->Render() | hcenter);
                content.push_back(separator());
                content.push_back(text("You can now restart the device to apply the firmware.") | dim | hcenter);
            }
        }

        return vbox(content);
    });
}
