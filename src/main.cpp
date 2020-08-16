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
// #define FASTLED_ALLOW_INTERRUPTS 0 // https://github.com/FastLED/FastLED/issues/306
// #define FASTLED_ESP8266_DMA

#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
// #include "MediaPlayer.h"
// #include "SdFat.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
#include "LittleFS.h"

#include "config.h"            // Set up the LED matrix here

#include "lib/gif/GifDecoder.h"

// #include "PongClock.h"

// Web interface
#include "webserver.h"

// SD Card setup
// SdFat sd;
// SdFile dirFile;
// File fd;

bool
  wifiConnected = false;

#define SD_CS_PIN 4

// LED setup
#define FRAMES_PER_SECOND 60 // TODO: implement

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

// PixelFrame::PongClockClass *pongClock  = new PixelFrame::PongClockClass(*matrix, timeClient, sd);

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))


String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

void startLittleFS() { // Start the LittleFS and list all contents
  Serial.println(F("Inizializing FS..."));
  if (LittleFS.begin()){
      Serial.println(F("done."));
  }else{
      Serial.println(F("fail."));
  }
  
  FSInfo fs_info;
  LittleFS.info(fs_info);

  Serial.println("File sistem info.");

  Serial.print("Total space:      ");
  Serial.print(fs_info.totalBytes);
  Serial.println("byte");

  Serial.print("Total space used: ");
  Serial.print(fs_info.usedBytes);
  Serial.println("byte");

  Serial.print("Block size:       ");
  Serial.print(fs_info.blockSize);
  Serial.println("byte");

  Serial.print("Page size:        ");
  Serial.print(fs_info.totalBytes);
  Serial.println("byte");

  Serial.print("Max open files:   ");
  Serial.println(fs_info.maxOpenFiles);

  Serial.print("Max path lenght:  ");
  Serial.println(fs_info.maxPathLength);

  Serial.println();
  
  Serial.println("LittleFS started. Contents:");
  {
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

// Giftest

// If the matrix is a different size than the GIFs, allow panning through the GIF
// while displaying it, or bouncing it around if it's smaller than the display
int OFFSETX = 0;
int OFFSETY = 0;
int FACTX = 0;
int FACTY = 0;

const char *pathname = "/www/css/cubeslide.gif";

/* template parameters are maxGifWidth, maxGifHeight, lzwMaxBits
 * 
 * The lzwMaxBits value of 12 supports all GIFs, but uses 16kB RAM
 * lzwMaxBits can be set to 10 or 11 for small displays, 12 for large displays
 * All 32x32-pixel GIFs tested work with 11, most work with 10
 */
GifDecoder<kMatrixWidth, kMatrixHeight, 12> decoder;

fs::File file;
bool fileSeekCallback(unsigned long position) { return file.seek(position); }
unsigned long filePositionCallback(void) { return file.position(); }
int fileReadCallback(void) { return file.read(); }
int fileReadBlockCallback(void * buffer, int numberOfBytes) { return file.read((uint8_t*)buffer, numberOfBytes); }

void screenClearCallback(void) { matrix->clear(); }
void updateScreenCallback(void) { matrix->show(); }

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
  CRGB color = CRGB(matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
  // This works but does not handle out of bounds pixels well (it writes to the last pixel)
  // matrixleds[XY(x+OFFSETX,y+OFFSETY)] = color;
  // drawPixel ensures we don't write out of bounds
  matrix->drawPixel(x+OFFSETX, y+OFFSETY, color);
}
// END Giftest


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(74880);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);

  startLittleFS();

  // FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is assumed
  // FastLED.setBrightness(BRIGHTNESS);

  // FastLED.addLeds<WS2812B, DATA_PIN>(matrixleds, NUMMATRIX); 
  // matrix->begin();
  // matrix->setTextWrap(false);
  // matrix->setBrightness(matrix_brightness);
  // matrix->setTextColor(colors[0]);

  // clearStripBuffer();
  // fastledshow();

  // matrix->clear();
  // End Black screen

  // stdout to serial setup
  hal_printf_init();

  Serial.print("Initializing SD card... ");
  // if (!sd.begin(SD_CS_PIN)) {
  //   error("initialization failed!");
  //   return;
  // }
  Serial.println("ok");


  // MediaPlayer.setup(leds, &sd);

  // Switch to bmp
  // MediaPlayer.play("system/nowifi.gif");

  // ### READ CONFIG
  Serial.print("Opening configuration file... ");
  // if (!fd.open("system/config.json", O_READ))
  // {
  //   error("opening config failed");
  //   return;
  // }
  Serial.println("ok");

  // Serial.println("reading json config");
  // char jsonData[fd.size() + 1];
  // int stringIndex = 0;
  // int data;
  // while ((data = fd.read()) >= 0)
  // {
  //   jsonData[stringIndex] = data;
  //   stringIndex++;
  // }
  // jsonData[stringIndex] = '\0'; // Add the NULL
  // fd.close();

  // // Deserialize the JSON document
  // DeserializationError error = deserializeJson(configuration, jsonData);
  // if (error)
  //   Serial.println(F("Failed to read configuration file"));

  // // ### END: READ CONFIG

  // // ### CONNECT WIFI
  // const char* ssid = configuration["wifi"]["ssid"];
  // const char* password = configuration["wifi"]["password"];

  // Serial.println("Clear screen");

  Serial.println("Connect wifi");

  // WiFi.begin(ssid, password);

  // while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
  //   delay(500);
  //   Serial.print('.');
  // }
  // Serial.println(WiFi.localIP());

  // timeClient.begin();

  // setup_webserver();

  // MediaPlayer.play("/system/nowifi.gif");

  // if (!dirFile.open("/")) {
  //   error("open root failed");
  // }

  // initClock();
  // pongClock->setup();

  decoder.setScreenClearCallback(screenClearCallback);
  decoder.setUpdateScreenCallback(updateScreenCallback);
  decoder.setDrawPixelCallback(drawPixelCallback);

  decoder.setFileSeekCallback(fileSeekCallback);
  decoder.setFilePositionCallback(filePositionCallback);
  decoder.setFileReadCallback(fileReadCallback);
  decoder.setFileReadBlockCallback(fileReadBlockCallback);

  Serial.println("Starting AnimatedGIFs Sketch");
  matrix_setup();

  Serial.print(pathname);

  if (file) file.close();
  file = LittleFS.open(pathname, "r");
  if (!file) {
    Serial.println(": Error opening GIF file");
    while (1) { delay(1000); }; // while 1 loop only triggers watchdog on ESP chips
  }
  Serial.println(": Opened GIF file, start decoding");
  decoder.startDecoding();
}

// long lastClockMillis = 0;
void loop() {
  // //
  // MediaPlayer.loop();
  // // delay(2);

  // webserver_loop();

  // MediaPlayer.stop();
  // timeClient.update();

  // showClock();
  // pongClock->loop();

  decoder.decodeFrame();
  // matrix->show();

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
