#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <atomic>
#include "libcontrol.h"
#include "rpc/RemoteDebugging.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class TaskManagerTab {
public:
    TaskManagerTab(std::shared_ptr<RemoteDebugging> remote_debugging, uint8_t device_id);
    Component createComponent();

private:
    std::shared_ptr<RemoteDebugging> remote_debugging_;
    uint8_t device_id_;

    // Caching for RemoteDebugging data to avoid frequent RPC calls
    mutable std::string cached_task_data_;
    mutable std::chrono::time_point<std::chrono::system_clock> task_data_last_updated_;
    mutable std::atomic<bool> task_data_loading_{false};
    mutable bool task_data_initial_load_done_{false};
    static constexpr std::chrono::seconds cache_duration_{4}; // 4 seconds

    std::string getCachedTaskData() const;
};
