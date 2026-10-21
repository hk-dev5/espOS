# ESP-OS

**A lightweight web-based desktop environment for ESP32.**

ESP-OS is an experimental project that turns an ESP32 into a small Wi-Fi-hosted desktop environment, accessible from a phone or computer browser.

The goal is to create a Raspberry Pi OS-inspired interface without running Linux, using the ESP32 as a standalone web server.

## Features

- **Desktop UI** - A desktop-like interface with a taskbar and application windows.
- **Wi-Fi Access Point** - Connect directly to the ESP32 without a router.
- **GPIO Control** - Configure supported GPIO pins as inputs or outputs and control their logic levels.
- **System Settings** - Basic interface and network information.
- **Mini Browser** - A small embedded browser window using the host browser's iframe support.
- **Offline Operation** - The local interface and GPIO controls work without Internet access.

## Hardware

Initial target:
- ESP32-C3 Super Mini

Other ESP32 boards may work with suitable pin configuration changes.

## Requirements

- Visual Studio Code
- PlatformIO IDE extension
- PlatformIO ESP32 platform and toolchain
- A compatible ESP32 board

## Installation

1. Clone or download this repository.
2. Open the project folder in Visual Studio Code.
3. Build the firmware using PlatformIO.
4. Connect your ESP32 board and select the correct upload port.
5. Upload the firmware.
6. Connect to the `ESP-OS` Wi-Fi network.
7. Open the following address in your browser:

   `http://192.168.4.1`

### Default Wi-Fi Credentials

- **SSID:** `ESP-OS`
- **Password:** `esp-os1234`

These are development defaults defined in the source code. Change them before sharing or deploying a build in an environment where others may connect.

## Project Structure

```text
ESP-OS/
├── platformio.ini
└── src/
    └── main.cpp
```

## Current Limitations

ESP-OS is an early prototype, not a standalone operating system.

- The desktop runs in a web browser.
- The mini browser depends on iframe support; some websites block embedded browsing.
- GPIO support is currently limited to basic digital input and output.
- Available GPIO pins depend on the board and its hardware connections.
- Settings are not yet fully persistent.
- The GPIO API does not currently provide dedicated authentication.

## Roadmap

- [ ] Improve desktop UI and window management
- [ ] Add persistent settings
- [ ] Add PWM and ADC controls
- [ ] Add more GPIO peripherals
- [ ] Improve browser and navigation features
- [ ] Add authentication and API security
- [ ] Test compatibility with more ESP32 boards

## Contributing

Contributions, bug reports, UI improvements, feature ideas, and hardware compatibility tests are welcome.

Feel free to fork the repository and experiment with your own ideas.

## License

This project is released under the MIT License. See [LICENSE](LICENSE) for details.

---

**ESP-OS is built for experimentation, learning, and community development.**
