/*
  PixelFrame

  Hardware connection:
  LED_DATA_PIN - D7
  LED_CLK_PIN - D5, if required, see confg.hpp,34
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
#include "fonts/TomThumbPatched.h"

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
  delay(500);

  // stdout to serial setup
  hal_printf_init();

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

  // Setup matrix
  // matrix_brightness = configuration["brightness"];
  matrix_setup();

  matrix->drawRect(2,5,12,6, matrix->Color(155, 155, 155));
  matrix->show();

  // ### CONNECT WIFI
  const char* ssid = configuration["wifi"]["ssid"];
  const char* password = configuration["wifi"]["password"];

  Serial.print(F("[WIFI] Connecting wifi"));
  WiFi.hostname("Pixelframe");
  WiFi.begin(ssid, password);

  matrix->drawRect(3,6,2,4, matrix->Color(155, 210, 155));
  matrix->show();

  // Wait for the Wi-Fi to connect
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print('.');
  }
  Serial.println("");
  Serial.print("[WIFI] IP: ");
  Serial.println(WiFi.localIP());

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

unsigned long _timer = millis();

void loop() {
  webserver_loop();
  Orchestrator::Instance()->loop();
}
