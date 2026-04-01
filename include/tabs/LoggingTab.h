#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <atomic>
#include "libcontrol.h"
#include "rpc/RemoteDebugging.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class LoggingTab {
public:
    LoggingTab(std::shared_ptr<RemoteDebugging> remote_debugging, uint8_t device_id);
    Component createComponent();

private:
    std::shared_ptr<RemoteDebugging> remote_debugging_;
    uint8_t device_id_;

    // Caching for RemoteDebugging data to avoid frequent RPC calls
    mutable std::string cached_logs_;
    mutable std::chrono::time_point<std::chrono::system_clock> logs_last_updated_;
    mutable std::atomic<bool> logs_loading_{false};
    mutable bool logs_initial_load_done_{false};
    static constexpr std::chrono::seconds cache_duration_{4}; // 4 seconds

    std::string getCachedLogs() const;
    std::string sanitizeLogData(const std::string& raw_data);
    std::vector<std::string> splitLogLines(const std::string& log_data);
};
