/*
  PixelFrame

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 13 - D7
 ** MISO - pin 12 - D6
 ** CLK - pin 14 - D5
 ** CS - pin 4 - D2

 TODO: LED connection
 LED - pin 5 - D1

 created   July 2020
 by Christian Erhardt
 */
// #define _stackSize (6748/4)
#define FS_NO_GLOBALS // otherwise there is a conflict between fs::File and SD File
#include "SPI.h"
#include "lib/stdinout.h"
// #define FASTLED_ALLOW_INTERRUPTS 0 // https://github.com/FastLED/FastLED/issues/306
// #define FASTLED_ESP8266_DMA

#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
// #include "MediaPlayer.h"
#include "SdFat.h"
#include "NTPClient.h"
#include "WiFiUdp.h"

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include "FastLED.h"
#include "PongClock.h"



#define MATRIX_TILE_WIDTH   16 // width of EACH NEOPIXEL MATRIX (not total display)
#define MATRIX_TILE_HEIGHT  16 // height of each matrix
#define MATRIX_TILE_H       1  // number of matrices arranged horizontally
#define MATRIX_TILE_V       1  // number of matrices arranged vertically

// Used by NeoMatrix
#define mw (MATRIX_TILE_WIDTH *  MATRIX_TILE_H)
#define mh (MATRIX_TILE_HEIGHT * MATRIX_TILE_V)
#define NUMMATRIX (mw*mh)

uint8_t matrix_brightness = 32;

CRGB matrixleds[NUMMATRIX];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V, 
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG + 
  NEO_TILE_TOP + NEO_TILE_LEFT +  NEO_TILE_PROGRESSIVE);

const uint16_t colors[] = {
  matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255) };

// SD Card setup
SdFat sd;
// SdFile dirFile;
File fd;

bool
  wifiConnected = false;

#define SD_CS_PIN 4

// LED setup
#define FRAMES_PER_SECOND 60 // TODO: implement
#define DATA_PIN          5
#define BRIGHTNESS        48
// #define CANVAS_WIDTH      16
// #define CANVAS_HEIGHT     16
// #define NUM_LEDS          (CANVAS_WIDTH * CANVAS_HEIGHT)
// CRGB leds[NUM_LEDS];
// CRGB leds_buf[NUM_LEDS];

StaticJsonDocument<512> configuration;

WiFiUDP ntpUDP;
// TODO: implement daylight savings time, etc
NTPClient timeClient(ntpUDP, 3600*2);


PixelFrame::PongClockClass *pongClock = new PixelFrame::PongClockClass(*matrix, timeClient, sd);

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);

  // FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is assumed
  // FastLED.setBrightness(BRIGHTNESS);

  FastLED.addLeds<WS2812B,DATA_PIN>(matrixleds, NUMMATRIX); 
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(40);
  // matrix->setTextColor(colors[0]);

  // clearStripBuffer();
  // fastledshow();

  matrix->clear();
  // End Black screen

  // stdout to serial setup
  hal_printf_init();

  Serial.print("Initializing SD card... ");
  if (!sd.begin(SD_CS_PIN)) {
    error("initialization failed!");
    return;
  }
  Serial.println("ok");


  // MediaPlayer.setup(leds, &sd);

  // Switch to bmp
  // MediaPlayer.play("system/nowifi.gif");

  // ### READ CONFIG
  Serial.print("Opening configuration file... ");
  if (!fd.open("system/config.json", O_READ))
  {
    error("opening config failed");
    return;
  }
  Serial.println("ok");

  Serial.println("reading json config");
  char jsonData[fd.size() + 1];
  int stringIndex = 0;
  int data;
  while ((data = fd.read()) >= 0)
  {
    jsonData[stringIndex] = data;
    stringIndex++;
  }
  jsonData[stringIndex] = '\0'; // Add the NULL
  fd.close();

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(configuration, jsonData);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // ### END: READ CONFIG

  // ### CONNECT WIFI
  const char* ssid = configuration["wifi"]["ssid"];
  const char* password = configuration["wifi"]["password"];

  // Serial.println("Clear screen");

  Serial.println("Connect wifi");

  WiFi.begin(ssid, password);

  // timeClient.begin();

  // MediaPlayer.play("/system/nowifi.gif");

  // if (!dirFile.open("/")) {
  //   error("open root failed");
  // }

  // initClock();
  pongClock->setup();
}

long lastClockMillis = 0;

void loop() {
  // //
  // MediaPlayer.loop();
  // // delay(2);

  // Wifi Connection
  if (!wifiConnected) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Wifi connected!");
      wifiConnected = true;
      // MediaPlayer.stop();
      // MediaPlayer.play("/system/wifi.gif");
    }
  } else {
    // MediaPlayer.stop();
    timeClient.update();

    if (millis() - lastClockMillis > 20) {
      // showClock();
      pongClock->loop();
      lastClockMillis = millis();
    }
  }
}
