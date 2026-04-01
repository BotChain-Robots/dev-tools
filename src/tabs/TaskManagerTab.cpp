#include "tabs/TaskManagerTab.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <thread>

TaskManagerTab::TaskManagerTab(std::shared_ptr<RemoteDebugging> remote_debugging, uint8_t device_id)
    : remote_debugging_(remote_debugging),
      device_id_(device_id),
      task_data_last_updated_(std::chrono::time_point<std::chrono::system_clock>{}) {
}

Component TaskManagerTab::createComponent() {
    return Renderer([=, this] {
        // Get cached task manager data (refreshed at most every 4 seconds)
        std::string task_data = getCachedTaskData();

        Elements content = {
            text("Task Manager") | bold | hcenter,
            separator()
        };

        // Check if we're still loading the initial data
        if (task_data_loading_ && !task_data_initial_load_done_) {
            content.push_back(text("Loading task manager data...") | color(Color::Yellow) | hcenter);
            return vbox(content);
        }

        if (task_data.empty()) {
            content.push_back(text("No task manager data available or device not responding") | color(Color::Yellow) | hcenter);
        } else {
            content.push_back(
                paragraph(task_data) | vscroll_indicator | frame | flex
            );
        }

        return vbox(content);
    });
}

std::string TaskManagerTab::getCachedTaskData() const {
    auto now = std::chrono::system_clock::now();
    auto time_since_update = std::chrono::duration_cast<std::chrono::seconds>(now - task_data_last_updated_);

    // If this is the first load and not already loading, start async fetch
    if (!task_data_initial_load_done_ && !task_data_loading_) {
        task_data_loading_ = true;
        std::thread([this]() {
            cached_task_data_ = remote_debugging_->get_task_manager(device_id_);
            task_data_last_updated_ = std::chrono::system_clock::now();
            task_data_initial_load_done_ = true;
            task_data_loading_ = false;
            // Force screen refresh to show the loaded data
            auto screen = ScreenInteractive::Active();
            if (screen) {
                screen->Post(Event::Custom);
            }
        }).detach();
        return ""; // Return empty on first call, will show loading message
    }

    // Update cache if it's been more than 4 seconds and not currently loading
    if (time_since_update >= cache_duration_ && !task_data_loading_) {
        task_data_loading_ = true;
        std::thread([this]() {
            cached_task_data_ = remote_debugging_->get_task_manager(device_id_);
            task_data_last_updated_ = std::chrono::system_clock::now();
            task_data_loading_ = false;
        }).detach();
    }

    return cached_task_data_;
}
