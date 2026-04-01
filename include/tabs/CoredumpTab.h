#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <atomic>
#include "libcontrol.h"
#include "rpc/RemoteDebugging.h"
#include <ftxui/component/component.hpp>

using namespace ftxui;

class CoredumpTab {
public:
    CoredumpTab(std::shared_ptr<RemoteDebugging> remote_debugging, uint8_t device_id);
    Component createComponent();

private:
    std::shared_ptr<RemoteDebugging> remote_debugging_;
    uint8_t device_id_;

    // Caching for coredump data to avoid frequent RPC calls
    mutable std::string cached_coredump_;
    mutable std::chrono::time_point<std::chrono::system_clock> coredump_last_updated_;
    mutable std::atomic<bool> coredump_loading_{false};
    mutable bool coredump_initial_load_done_{false};
    static constexpr std::chrono::seconds cache_duration_{10}; // 10 seconds

    std::string getCachedCoredump() const;
    std::string sanitizeData(const std::string& raw_data);
    std::vector<std::string> splitLines(const std::string& data);
};
