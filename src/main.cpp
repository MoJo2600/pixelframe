/*
  PixelFrame

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 13 - D7
 ** MISO - pin 12 - D6
 ** CLK  - pin 14 - D5
 ** CS   - pin 4  - D2

 TODO: LED connection
 LED - pin 5 - D1

 created   July 2020
 by Christian Erhardt
 */
// #define _stackSize (6748/4)
#include "SPI.h"
#include "lib/stdinout.h"
// #define FASTLED_ALLOW_INTERRUPTS 0  // https://github.com/FastLED/FastLED/issues/306
// #define FASTLED_ESP8266_DMA

#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
//#include "NTPClient.h"
#include "WiFiUdp.h"
#include "config.h"                     // Set up the LED matrix here
#include "ezTime.h"
#include "webserver.h"                  // Web interface
#include "pixelframe.hpp"               // Statemachine
#include <filesystem.hpp>
#include "mqtt.h"                  // MQTT support

bool
  wifiConnected = false;

#define SD_CS_PIN 4

StaticJsonDocument<512> configuration;

Timezone * tz = new Timezone();

// Statemachine handle
FastLED_NeoMatrix * PixelframeStateMachine::pixel_matrix{matrix};
Timezone * PixelframeStateMachine::timezone{tz};
using fsm_handle = PixelframeStateMachine;

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

fs::File file;

// instantiate events
ToggleEvent toggle;
LoopEvent loopUpdate;


// mqtt subscription callback. This function is called when new messages arrive at the client.
void my_mqtt_callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  String pattern = doc["pattern"]; // e.g. "blink_slowly"
  JsonArray color = doc["color"];
  int R = color[0]; // e.g. 255
  int G = color[1]; // e.g. 255
  int B = color[2]; // e.g. 255
  int duration = doc["duration"]; // e.g. 10

  // Print received MQTT message. TODO: format JSON
  Serial.print("[MQTT] message on (");
  Serial.print(topic);
  Serial.println("): ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(74880);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);

  // stdout to serial setup
  hal_printf_init();

  // Setup matrix
  matrix_setup();
  matrix->clear();

  startLittleFS();

  // ### READ CONFIG
  Serial.print("Opening configuration file... ");
  file = LittleFS.open("system/config.json", "r");
  Serial.println("ok");

  Serial.println("reading json config");
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
    Serial.println(F("Failed to read configuration file"));

  // ### END: READ CONFIG

  // ### CONNECT WIFI
  const char* ssid = configuration["wifi"]["ssid"];
  const char* password = configuration["wifi"]["password"];

  Serial.println("Connect wifi");
  WiFi.begin(ssid, password);

  // Wait for the Wi-Fi to connect
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print('.');
  }
  Serial.println(WiFi.localIP());

  // Read MQTT settings
  const char* mqtt_host = configuration["mqtt"]["host"];
  const unsigned int mqtt_port = configuration["mqtt"]["port"];

  mqtt_setup(mqtt_host, mqtt_port, my_mqtt_callback);

  // Time
  Serial.println("Adjusting Clock");
  const char* tzConf = configuration["timezone"];
  waitForSync();
  tz->setLocation(tzConf);
  Serial.println(tz->dateTime());
  Serial.println((String)"Timezone: " + tzConf);

  setup_webserver();

  fsm_handle::start();
}



unsigned long _timer = millis();
void loop() {
  if ((millis() - _timer) >= 10*1000) {  // 1*1000
    fsm_handle::dispatch(toggle);
    Serial.println("Memory after state switch");
    show_free_mem();
    _timer = millis();
  };

  // should be checking for MQTT connection and reconnect
  if (!mqtt_connected()) {
    mqtt_connect(configuration["mqtt"]["user"], configuration["mqtt"]["password"], configuration["mqtt"]["topic"]);
  }
  mqtt_loop();

  webserver_loop();

  fsm_handle::dispatch(loopUpdate);

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
