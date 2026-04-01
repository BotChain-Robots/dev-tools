#include "tabs/CoredumpTab.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <thread>

CoredumpTab::CoredumpTab(std::shared_ptr<RemoteDebugging> remote_debugging, uint8_t device_id)
    : remote_debugging_(remote_debugging),
      device_id_(device_id),
      coredump_last_updated_(std::chrono::time_point<std::chrono::system_clock>{}) {
}

Component CoredumpTab::createComponent() {
    return Renderer([=, this] {
        std::string coredump_data = getCachedCoredump();

        Elements content = {
            text("Core Dump") | bold | hcenter,
            separator()
        };

        if (coredump_loading_ && !coredump_initial_load_done_) {
            content.push_back(text("Loading core dump...") | color(Color::Yellow) | hcenter);
            return vbox(content);
        }

        if (coredump_data.empty()) {
            content.push_back(text("No core dump available or device not responding") | color(Color::Yellow) | hcenter);
        } else {
            std::string sanitized = sanitizeData(coredump_data);
            std::vector<std::string> lines = splitLines(sanitized);

            Elements line_elements;
            for (const auto& line : lines) {
                if (line.empty()) {
                    line_elements.push_back(text(" ") | dim);
                } else {
                    line_elements.push_back(text(line));
                }
            }

            if (line_elements.empty()) {
                content.push_back(text("No data found after processing") | color(Color::Yellow) | hcenter);
            } else {
                content.push_back(
                    vbox(line_elements) | vscroll_indicator | frame | flex
                );
            }
        }

        return vbox(content);
    });
}

std::string CoredumpTab::getCachedCoredump() const {
    auto now = std::chrono::system_clock::now();
    auto time_since_update = std::chrono::duration_cast<std::chrono::seconds>(now - coredump_last_updated_);

    if (!coredump_initial_load_done_ && !coredump_loading_) {
        coredump_loading_ = true;
        std::thread([this]() {
            cached_coredump_ = remote_debugging_->get_coredump(device_id_);
            coredump_last_updated_ = std::chrono::system_clock::now();
            coredump_initial_load_done_ = true;
            coredump_loading_ = false;
            auto screen = ScreenInteractive::Active();
            if (screen) {
                screen->Post(Event::Custom);
            }
        }).detach();
        return "";
    }

    if (time_since_update >= cache_duration_ && !coredump_loading_) {
        coredump_loading_ = true;
        std::thread([this]() {
            cached_coredump_ = remote_debugging_->get_coredump(device_id_);
            coredump_last_updated_ = std::chrono::system_clock::now();
            coredump_loading_ = false;
        }).detach();
    }

    return cached_coredump_;
}

std::string CoredumpTab::sanitizeData(const std::string& raw_data) {
    std::string sanitized;
    sanitized.reserve(raw_data.size());

    for (char c : raw_data) {
        if ((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t') {
            sanitized += c;
        } else if (c < 32 && c != '\n' && c != '\r' && c != '\t') {
            sanitized += '?';
        }
    }

    return sanitized;
}

std::vector<std::string> CoredumpTab::splitLines(const std::string& data) {
    std::vector<std::string> lines;
    std::string current_line;

    for (size_t i = 0; i < data.size(); ++i) {
        char c = data[i];
        if (c == '\n') {
            lines.push_back(current_line);
            current_line.clear();
        } else if (c == '\r') {
            lines.push_back(current_line);
            current_line.clear();
            if (i + 1 < data.size() && data[i + 1] == '\n') {
                ++i;
            }
        } else {
            current_line += c;
        }
    }

    lines.push_back(current_line);
    return lines;
}
