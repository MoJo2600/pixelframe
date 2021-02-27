# PixelFrame

pixelframe - a box w/ a DIY 16x16 LED matrix, that displays low res images, gif. Based on a ESP8266 SoC.

![Pixelframe](docs/pixelframe.png "Pixelframe")

## Features

- Pong Clock
- Play Gifs based on API events / MQTT messages (in development)
- Web based management interface (in development)

## Components

### ESP8266 Firmware

The firmware code is found in the base directory. A vscode development container can be used for easier development.

### Webfrontend

The code of the web frontend can be found in the `webfrontend` folder. A vscode development container can be used for easier development.

## How to build & deploy

### Using Platform IO and VS Code

**Hint**: This setup does not work with WSL without a lot of effort because WSL does not handle USB devices by default.

1. Open VS Code, go to extensions and install the `PlatformIO IDE` extension. Alternatively, run following command:

```bash
code --install-extension platformio.platformio-ide
```

**Remark**: On Windows, an error can occur during installation (https://github.com/platformio/platformio-core-installer/issues/221). Follow those steps if the installation fails and reinstall the extension.

2. Install drivers for USB to UART bridge for your operating system. In our case, using an ESP8266 with a CP2104 USB to UART bridge, the drivers are located here: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

**Remark**: Missing drivers can be detected by seeing unknown devices in the Device Manager on Windows or the following error message during deployment: `"Error: Please specify upload_port for environment or use global --upload-port option."`

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

## Used sources

https://github.com/Jerware/GameFrameV2
https://github.com/pixelmatix/AnimatedGIFs
https://github.com/marcmerlin/AnimatedGIFs

## Useful links

https://tchapi.github.io/Adafruit-GFX-Font-Customiser/
