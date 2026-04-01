#include "tabs/InformationTab.h"
#include <ftxui/dom/elements.hpp>
#include <format>

InformationTab::InformationTab(std::shared_ptr<RobotController> robot_controller, uint8_t device_id)
    : robot_controller_(robot_controller), device_id_(device_id) {
}

Component InformationTab::createComponent() {
    return Renderer([=, this] {
        // Try to get the module data
        auto module_opt = robot_controller_->getModule(device_id_);

        Elements content = {
            text("Device Information") | bold | hcenter,
            separator()
        };

        if (!module_opt.has_value()) {
            content.push_back(text("Module not found or unavailable") | color(Color::Red) | hcenter);
            return vbox(content);
        }

        auto module_weak = module_opt.value();
        if (module_weak.expired()) {
            content.push_back(text("Module reference expired") | color(Color::Red) | hcenter);
            return vbox(content);
        }

        auto module = module_weak.lock();
        if (!module) {
            content.push_back(text("Failed to access module") | color(Color::Red) | hcenter);
            return vbox(content);
        }

        // Get real module data
        uint8_t device_id = module->get_device_id();
        ModuleType module_type = module->get_type();
        Messaging::ConnectionType connection_type = module->get_connection_type();
        uint8_t leader = module->get_leader();
        auto last_updated = module->get_last_updated_time();
        auto neighbours = module->get_neighbours();

        // Create information display with real data
        Elements info_elements = {
            hbox({text("Device ID: ") | bold, text(std::to_string(static_cast<int>(device_id)))}),
            hbox({text("Module Type: ") | bold, text(getModuleTypeString(module_type))}),
            hbox({text("Connection Type: ") | bold, text(getConnectionTypeString(connection_type))}),
            hbox({text("Leader ID: ") | bold, text(std::to_string(static_cast<int>(leader)))}),
            hbox({text("Last Updated: ") | bold, text(formatTimeSince(last_updated))}),
            separator(),
            text("Neighbours:") | bold
        };

        // Add neighbour information
        if (neighbours.empty()) {
            info_elements.push_back(text("• No neighbours detected") | dim);
        } else {
            for (const auto& neighbour : neighbours) {
                info_elements.push_back(
                    text(std::format("• Device ID: {} (Orientation: {})",
                         static_cast<int>(neighbour.device_id),
                         static_cast<int>(neighbour.orientation))) | dim
                );
            }
        }

        info_elements.push_back(separator());

        // Try to get sensor values (these may throw or return defaults if not implemented)
        try {
            double position = module->get_position();
            std::string text_value = module->get_text();

            info_elements.push_back(text("Sensor Values:") | bold);
            if (position != 0.0) {  // Assume 0.0 means no position data
                info_elements.push_back(
                    text(std::format("• Position: {:.2f}", position)) | dim
                );
            }
            if (!text_value.empty()) {
                info_elements.push_back(
                    text(std::format("• Text: {}", text_value)) | dim
                );
            }
            if (position == 0.0 && text_value.empty()) {
                info_elements.push_back(text("• No sensor data available") | dim);
            }
        } catch (...) {
            info_elements.push_back(text("• Sensor data unavailable") | dim);
        }

        content.insert(content.end(), info_elements.begin(), info_elements.end());

        return vbox(content) | flex;
    });
}

std::string InformationTab::getModuleTypeString(ModuleType type) {
    // Provide meaningful names for common module types
    int type_value = static_cast<int>(type);
    switch (type_value) {
        case 0: return "MMMF Splitter";
        case 1: return "Arm Servo";
        case 2: return "DC Motor";
        case 3: return "Power";
        case 4: return "Pivot Servo";
        case 5: return "Display";
        case 6: return "Gripper";
        case 7: return "Speaker";
        case 8: return "IMU";
        case 9: return "Distance Sensor";
        case 10: return "MMMM Splitter";
        case 11: return "MMF Triangle Splitter";
        case 12: return "MMM Triangle Splitter";
        default: return std::format("Module Type {}", type_value);
    }
}

std::string InformationTab::getConnectionTypeString(Messaging::ConnectionType type) {
    // Provide meaningful names for common connection types
    int type_value = static_cast<int>(type);
    switch (type_value) {
        case 0: return "Wi-Fi";
        case 1: return "Wired Hop";
        default: return std::format("Connection Type {}", type_value);
    }
}

std::string InformationTab::formatTimeSince(const std::chrono::time_point<std::chrono::system_clock>& time_point) {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - time_point);

    auto seconds = duration.count();

    if (seconds < 60) {
        return std::format("{} seconds ago", seconds);
    } else if (seconds < 3600) {
        auto minutes = seconds / 60;
        return std::format("{} minute{} ago", minutes, minutes == 1 ? "" : "s");
    } else if (seconds < 86400) {
        auto hours = seconds / 3600;
        return std::format("{} hour{} ago", hours, hours == 1 ? "" : "s");
    } else {
        auto days = seconds / 86400;
        return std::format("{} day{} ago", days, days == 1 ? "" : "s");
    }
}
