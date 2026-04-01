
#include <iostream>
#include "DeviceListApp.h"

int main() {
    try {
        DeviceListApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
