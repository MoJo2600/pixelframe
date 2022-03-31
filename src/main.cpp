/*
  PixelFrame

  Hardware connection:
  LED_DATA_PIN - D7
  LED_CLK_PIN - D5, if required, see confg.hpp,34
 */

#define FASTLED_INTERNAL                    // Remove Fastled warnings
// https://github.com/FastLED/FastLED/issues/1169
// TODO: #define FASTLED_ALL_PINS_HARDWARE_SPI
// Should be irrelevant, because SPI is never used for WS8xxx LEDs
// #define FASTLED_ESP32_SPI_BUS HSPI
#include "Preferences.h"
// #include "WiFiClientSecure.h"
#include "SPI.h"
#include "lib/stdinout.h"
#include "ArduinoJson.h"
// #include "WiFiUdp.h"
#include "config.hpp"                    // Set up the LED matrix here

#include "pixel_webserver.h"                   // Web interface
#include <filesystem.hpp>
#include "components/orchestrator.hpp"
#include "frames/frame.hpp"
#include "fonts/TomThumbPatched.h"

#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

bool
  wifiConnected = false;

StaticJsonDocument<512>
  configuration;

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

fs::File file;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

  // stdout to serial setup
  hal_printf_init();

  startLittleFS();

  // ### READ CONFIG
  Serial.print(F("[CONFIG] Opening configuration file.. "));
  file = LITTLEFS.open("/system/config.json", "r");
  Serial.println(F("ok"));
  Serial.println(F("[CONFIG] Reading json config.."));
  char jsonData[file.size() + 1];
  int stringIndex = 0;
  int data;
  while ((data = file.read()) >= 0)
  {
    jsonData[stringIndex] = data;
    stringIndex++;
  }
  jsonData[stringIndex] = '\0'; // Add the NULL
  file.close();

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(configuration, jsonData);
  if (error)
    Serial.println(F("[CONFIG] Failed to read configuration file!"));

  // ### END: READ CONFIG

  // Setup matrix
  if (configuration["brightness"] != nullptr) {
    matrix_brightness = configuration["brightness"];
  }

  if (configuration["defaultmode"] != nullptr) {
    default_mode = strdup(configuration["defaultmode"]);
    Serial.print(F("[CONFIG] Default mode: "));
    Serial.println(default_mode);
  }

  matrix_setup();

  // ### CONNECT WIFI
  // This is a sample setup... there are a lot of good examples on how to get configuration from json, etc:
  // https://github.com/tzapu/WiFiManager/tree/master/examples
  // 
  // WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;

  char randomPasswordStr[8];
  sprintf(randomPasswordStr, "%lu", random(10000000,99999999));
  Serial.print(F("[CONFIG] AP password: "));
  Serial.println(randomPasswordStr);

  // draw AP PW on screen
  matrix->setTextColor(matrix->Color(155, 155, 155));
  matrix->setTextSize(1);
  matrix->setTextWrap(false);
  matrix->setFont(&TomThumbPatched);
  // Text wrap breaks to early and shows only 3 characters... doh
  matrix->println(randomPasswordStr);
  matrix->print(&randomPasswordStr[4]);
  matrix->show();

  res = wm.autoConnect("Pixelframe", randomPasswordStr); // password protected ap

  if(!res) {
    Serial.println("Failed to connect");
    ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");

    matrix->drawRect(2,5,12,6, matrix->Color(155, 155, 155));
    matrix->show();

    mdnsName = strdup("pixelframe");
    if (configuration["framename"] != nullptr) {
      mdnsName = strdup(configuration["framename"]);
    }

    // WiFi.setHostname(mdnsName);

    matrix->drawRect(3,6,2,4, matrix->Color(155, 210, 155));
    matrix->show();

    if (wifi_ssid == nullptr || wifi_password == nullptr) {
      // TODO: access point
      // configure wifi via webpage
    } else {
      set_wifi(wifi_ssid, wifi_password);
    }

    matrix->drawRect(5,6,2,4, matrix->Color(155, 210, 155));
    matrix->show();

    // Time
    Serial.println(F("[TZ] Adjusting clock.."));
    const char* tzConf = configuration["timezone"];
    waitForSync();
    timezone->setLocation(tzConf);
    Serial.print(F("[TZ] "));
    Serial.println(timezone->dateTime());
    Serial.print(F("[TZ] Timezone: "));
    Serial.println(tzConf);

    matrix->drawRect(7,6,2,4, matrix->Color(155, 210, 155));
    matrix->show();

    setup_webserver();

    matrix->drawRect(9,6,2,4, matrix->Color(155, 210, 155));
    matrix->show();

    Orchestrator::Instance()->setup();

    matrix->drawRect(11,6,2,4, matrix->Color(155, 210, 155));
    matrix->show();
  }
}

unsigned long _timer = millis();

void loop() {
  webserver_loop();
  Orchestrator::Instance()->loop();
}
