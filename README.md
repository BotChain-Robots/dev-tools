# Developer Tools
A TUI to interface with BotChain devices.
Provides the following features:
- Device discovery
- View device metadata, sensor data, and connections.
- View logs from device.
- Apply an Over The Air (OTA) update.
- Change settings (device ID, type, wifi, etc.)
- View crash dumps from device

## Platform Support
- MacOS (Apple silicon)
- MacOS (x86)
- Ubuntu (x86)
- Windows (x86)

## Setup
### MacOS
Install xcode command line tools (if you do not already have them)
```
xcode-select --install
```

Install conan and dependencies
```
brew install conan cmake ninja clang-format@21
```

Generate a conan profile
```
conan profile detect --force
```

Follow the required artifactory setup.

### Ubuntu
On newer versions of Ubuntu, the package manager is responsible for managing python packages. We use `pipx` to create a virtual environment.

Install `pipx` and dependencies
```
sudo apt install pipx cmake ninja-build
```

Install clang-format version 21.

Install conan
```
pipx install conan
```

Generate a conan profile
```
conan profile detect --force
```

Follow the required artifactory setup.

### Required Artifactory Setup
These instructions should only be followed after you have completed all setup steps for your platform.

Add the artifactory
```
conan remote add artifactory http://jslightham.com:8081/artifactory/api/conan/botchain
```

## Running the App
```
# Macos and Linux users can run,
./build.sh
```
