/*
  PixelFrame

  Hardware connection:
  LED - D7
 */
#include "SPI.h"
#include "lib/stdinout.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "WiFiUdp.h"
#include "config.hpp"                    // Set up the LED matrix here
#include "ezTime.h"
#include "webserver.h"                   // Web interface
#include <filesystem.hpp>
#include "components/orchestrator.hpp"
#include "frames/frame.hpp"

bool
  wifiConnected = false;

StaticJsonDocument<512>
  configuration;

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

fs::File file;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(74880);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // stdout to serial setup
  hal_printf_init();

  // Setup matrix
  matrix_setup();
  matrix->clear();

  startLittleFS();

  // ### READ CONFIG
  Serial.print(F("[CONFIG] Opening configuration file.. "));
  file = LittleFS.open("system/config.json", "r");
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

  // ### CONNECT WIFI
  const char* ssid = configuration["wifi"]["ssid"];
  const char* password = configuration["wifi"]["password"];

  Serial.print(F("[WIFI] Connecting wifi"));
  WiFi.hostname("Pixelframe");
  WiFi.begin(ssid, password);

  // Wait for the Wi-Fi to connect
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print('.');
  }
  Serial.println("");
  Serial.print("[WIFI] IP: ");
  Serial.println(WiFi.localIP());

  // Time
  Serial.println(F("[TZ] Adjusting clock.."));
  const char* tzConf = configuration["timezone"];
  waitForSync();
  timezone->setLocation(tzConf);
  Serial.print(F("[TZ] "));
  Serial.println(timezone->dateTime());
  Serial.print(F("[TZ] Timezone: "));
  Serial.println(tzConf);

  setup_webserver();

  Orchestrator::Instance()->setup();
}

unsigned long _timer = millis();

void loop() {

  webserver_loop();

  Orchestrator::Instance()->loop();

#ifdef ESP8266
  // Disable watchdog interrupt so that it does not trigger in the middle of
  // updates. and break timing of pixels, causing random corruption on interval
  // https://github.com/esp8266/Arduino/issues/34
  ESP.wdtDisable();
#endif
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
#ifdef ESP8266
  ESP.wdtEnable(1000);
#endif

}
