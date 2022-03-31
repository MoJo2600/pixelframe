# PixelFrame

pixelframe - a box w/ a DIY 16x16 LED matrix, that displays low res images, gif. Based on a ESP8266 SoC.

![Pixelframe](docs/pixelframe.png "Pixelframe")

## Features

- Pong Clock
- Play Gifs based on API events / MQTT messages (in development)
- Web based management interface (in development)

## Components

### Hardware

We are using NodeMCU pro mini ESP8266 boards. Connect the LED strip to 5V, GND and the data pin to `D7`

### ESP32 Firmware

The firmware code is found in the base directory. A vscode development container can be used for easier development.

### Webfrontend

The code of the web frontend can be found in the `webfrontend` folder. A vscode development container can be used for easier development.

## How to build & deploy

### Using Platform IO and VS Code (Windows)

1. Open VS Code, go to extensions and install the `PlatformIO IDE` extension. Alternatively, run following command:

   ```bash
   code --install-extension platformio.platformio-ide
   ```
   
   **Remark**: On Windows, an error can occur during installation (https://github.   com/platformio/platformio-core-installer/issues/221). Follow those steps if the    installation fails and reinstall the extension.

2. Install drivers for USB to UART bridge for your operating system. In our case, using an ESP32 with a CP2104 USB to UART bridge, the drivers are located here: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

   **Remark**: Missing drivers can be detected by seeing unknown devices in the    Device Manager on Windows or the following error message during deployment:    `"Error: Please specify upload_port for environment or use global --upload-port    option."`

3. Open this projects root directory in VS Code

4. Set up data directory (within root folder of the project) with data for the filesystem on the ESP containing the following content:

   ```
   data
   │
   └─ gifs
   │  │  *.gif
   │
   └─ system
   │  │  config.json
   ```

   **config.json**

   ```json
   {
     "wifi": {
       "ssid": "ssid",
       "password": "password"
     },
     "timezone": "Europe/Berlin"
   }
   ```

5. Upload the filesystem to the ESP by going to the PlatformIO extension and selecting `Platform > Upload Filesystem Image`

6. Use the VS Code commands, assigned keybindings or icons in the footer for

   - building (check mark): `PlatformIO: Build`
   - deploying (arrow): `PlatformIO: Upload`

### Using Platform IO and VS Code (Windows WSL2)

Prerequisites: VSCode with installed PlatformIO extension. WSL with Ubuntu 20.04, [Docker installed inside WSL2 without Docker Desktop](https://dev.to/felipecrs/simply-run-docker-on-wsl2-3o8)

1. [Install usbipd.win](https://devblogs.microsoft.com/commandline/connecting-usb-devices-to-wsl/) and connect the USB device to WSL

   ```
   usbipd wsl list
   usbipd wsl attach --busid <busid>
   ```

2. Add user to dialout group

   `sudo usermod -a -G dialout $USER`

3. Change the device to be writable during boot




## Used sources

https://github.com/Jerware/GameFrameV2
https://github.com/pixelmatix/AnimatedGIFs
https://github.com/marcmerlin/AnimatedGIFs

## Useful links

https://tchapi.github.io/Adafruit-GFX-Font-Customiser/

Littlefs
https://github.com/lorol/LITTLEFS


Captive Portal
https://github.com/me-no-dev/ESPAsyncWebServer/blob/2f784268f0a358741ee6384480d48656e159d726/examples/CaptivePortal/CaptivePortal.ino