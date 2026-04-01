#include "tabs/LoggingTab.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <thread>

LoggingTab::LoggingTab(std::shared_ptr<RemoteDebugging> remote_debugging, uint8_t device_id)
    : remote_debugging_(remote_debugging),
      device_id_(device_id),
      logs_last_updated_(std::chrono::time_point<std::chrono::system_clock>{}) {
}

Component LoggingTab::createComponent() {
    return Renderer([=, this] {
        // Get cached log data (refreshed at most every 4 seconds)
        std::string log_data = getCachedLogs();

        Elements content = {
            text("Device Logs") | bold | hcenter,
            separator()
        };

        // Check if we're still loading the initial data
        if (logs_loading_ && !logs_initial_load_done_) {
            content.push_back(text("Loading logs...") | color(Color::Yellow) | hcenter);
            return vbox(content);
        }

        if (log_data.empty()) {
            content.push_back(text("No logs available or device not responding") | color(Color::Yellow) | hcenter);
        } else {
            // Sanitize the log data
            std::string sanitized_logs = sanitizeLogData(log_data);

            // Split the logs by newlines and create text elements for each line
            std::vector<std::string> log_lines = splitLogLines(sanitized_logs);

            Elements log_elements;
            for (size_t i = 0; i < log_lines.size(); ++i) {
                const auto& line = log_lines[i];
                if (line.empty()) {
                    // Show empty lines as a visual placeholder
                    log_elements.push_back(text(" ") | dim);
                } else {
                    log_elements.push_back(text(line));
                }
            }

            if (log_elements.empty()) {
                content.push_back(text("No log lines found after processing") | color(Color::Yellow) | hcenter);
            } else {
                content.push_back(
                    vbox(log_elements) | vscroll_indicator | frame | flex
                );
            }
        }

        return vbox(content);
    });
}

std::string LoggingTab::getCachedLogs() const {
    auto now = std::chrono::system_clock::now();
    auto time_since_update = std::chrono::duration_cast<std::chrono::seconds>(now - logs_last_updated_);

    // If this is the first load and not already loading, start async fetch
    if (!logs_initial_load_done_ && !logs_loading_) {
        logs_loading_ = true;
        std::thread([this]() {
            cached_logs_ = remote_debugging_->get_logs(device_id_);
            logs_last_updated_ = std::chrono::system_clock::now();
            logs_initial_load_done_ = true;
            logs_loading_ = false;
            // Force screen refresh to show the loaded data
            auto screen = ScreenInteractive::Active();
            if (screen) {
                screen->Post(Event::Custom);
            }
        }).detach();
        return ""; // Return empty on first call, will show loading message
    }

    // Update cache if it's been more than 4 seconds and not currently loading
    if (time_since_update >= cache_duration_ && !logs_loading_) {
        logs_loading_ = true;
        std::thread([this]() {
            cached_logs_ = remote_debugging_->get_logs(device_id_);
            logs_last_updated_ = std::chrono::system_clock::now();
            logs_loading_ = false;
        }).detach();
    }

    return cached_logs_;
}

std::string LoggingTab::sanitizeLogData(const std::string& raw_data) {
    std::string sanitized;
    sanitized.reserve(raw_data.size()); // Reserve space to avoid reallocations

    for (char c : raw_data) {
        // Allow printable ASCII characters and newlines
        if ((c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t') {
            sanitized += c;
        } else if (c < 32 && c != '\n' && c != '\r' && c != '\t') {
            // Replace non-printable control characters with a placeholder
            sanitized += '?';
        }
        // Skip characters outside ASCII range (>126)
    }

    return sanitized;
}

std::vector<std::string> LoggingTab::splitLogLines(const std::string& log_data) {
    std::vector<std::string> lines;
    std::string current_line;

    for (size_t i = 0; i < log_data.size(); ++i) {
        char c = log_data[i];

        if (c == '\n') {
            // Handle \n or \r\n
            lines.push_back(current_line);
            current_line.clear();
        } else if (c == '\r') {
            // Handle \r or \r\n
            lines.push_back(current_line);
            current_line.clear();
            // Skip the \n if it follows \r
            if (i + 1 < log_data.size() && log_data[i + 1] == '\n') {
                ++i;
            }
        } else {
            current_line += c;
        }
    }

    // Add the last line even if it doesn't end with a newline
    lines.push_back(current_line);

    return lines;
}
