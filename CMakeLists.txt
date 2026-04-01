cmake_minimum_required(VERSION 3.15)

project(DevTools)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    add_compile_options(/permissive-)
    add_compile_definitions(NOMINMAX)
endif()

find_package(libcontrol REQUIRED)
find_package(spdlog REQUIRED)
find_package(librpc REQUIRED)
find_package(ftxui REQUIRED)

add_executable(DevTools
    src/main.cpp
    src/DeviceListApp.cpp
    src/DeviceDetailsApp.cpp
    src/tabs/InformationTab.cpp
    src/tabs/LoggingTab.cpp
    src/tabs/FirmwareUpdateTab.cpp
    src/tabs/TaskManagerTab.cpp
    src/tabs/ConfigurationTab.cpp
    src/tabs/CoredumpTab.cpp
    src/flatbuffers/OTAPacketBuilder.cpp
    src/rpc/RemoteDebugging.cpp
    src/rpc/RemoteManagement.cpp
    src/rpc/RemoteConfig.cpp
)

target_include_directories(DevTools PUBLIC include)

target_link_libraries(DevTools
    PRIVATE
        libcontrol::libcontrol
        spdlog::spdlog
        librpc::librpc
        ftxui::ftxui
)
