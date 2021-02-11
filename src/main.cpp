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
//#include "NTPClient.h"
#include "WiFiUdp.h"
#include "LittleFS.h"
#include "config.h"            // Set up the LED matrix here
#include "lib/gif/GifDecoder.h"
#include "ezTime.h"
#include "PongClock.h"
// Web interface
#include "webserver.h"
#include "lib/tinyfsm.hpp"
#include "vector"
using namespace std;

// SD Card setup
// SdFat sd;
// SdFile dirFile;
// File fd;

bool
  wifiConnected = false;

#define SD_CS_PIN 4

// LED setup
#define FRAMES_PER_SECOND 60

PixelFrame::PongClockClass *pongClock;

// State machine
struct Gif; // forward declaration
struct ToggleEvent : tinyfsm::Event { };
struct LoopEvent : tinyfsm::Event { };

struct Switch : tinyfsm::Fsm<Switch>
{
  virtual void react(ToggleEvent const &) { };
  virtual void react(LoopEvent const &) { };
  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void)  { };  /* no exit actions */
  // alternative: enforce entry actions in all states (pure virtual)
  //virtual void entry(void) = 0;
};

// END - state machine

StaticJsonDocument<512> configuration;

// Timezone tz;
bool fsOK;

//WiFiUDP ntpUDP;
// TODO: implement daylight savings time, etc
//NTPClient timeClient(ntpUDP, 3600*2);

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

vector<String> gifs_vec;

void startLittleFS() { // Start the LittleFS and list all contents
  Serial.println(F("Inizializing FS..."));
  fsOK = LittleFS.begin();
  if (fsOK){
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

  Serial.print("Max path length:  ");
  Serial.println(fs_info.maxPathLength);

  Serial.println();
  
  Serial.println("LittleFS started. Contents:");
  {
    Dir dir = LittleFS.openDir("/gifs");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      gifs_vec.push_back("/gifs/"+fileName);
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

const char *pathname = "/gifs/bird.gif";

/* template parameters are maxGifWidth, maxGifHeight, lzwMaxBits
 * 
 * The lzwMaxBits value of 12 supports all GIFs, but uses 16kB RAM
 * lzwMaxBits can be set to 10 or 11 for small displays, 12 for large displays
 * All 32x32-pixel GIFs tested work with 11, most work with 10
 */
GifDecoder<kMatrixWidth, kMatrixHeight, 10> decoder;

fs::File file;
bool fileSeekCallback(unsigned long position) { return file.seek(position); }
unsigned long filePositionCallback(void) { return file.position(); }
int fileReadCallback(void) { return file.read(); }
int fileReadBlockCallback(void * buffer, int numberOfBytes) { return file.read((uint8_t*)buffer, numberOfBytes); }
void screenClearCallback(void) {
  //matrix->clear(); 
}
void updateScreenCallback(void) {
  //matrix->show(); 
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
  CRGB color = CRGB(matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
  // This works but does not handle out of bounds pixels well (it writes to the last pixel)
  // matrixleds[XY(x+OFFSETX,y+OFFSETY)] = color;
  // drawPixel ensures we don't write out of bounds
  matrix->drawPixel(x+OFFSETX, y+OFFSETY, color);
}
// END Giftest


struct Clock : Switch
{
  void entry() override { 
    Serial.println("On");
    pongClock  = new PixelFrame::PongClockClass(matrix, "Europe/Berlin"); // TODO: Read tz from config
    pongClock->setup();
  };
  void react(ToggleEvent const &) override {
    matrix->clear();
    transit<Gif>();
  };
  void react(LoopEvent const &) override { 
    pongClock->loop();
  };
};

struct Gif : Switch
{
  void entry() override { 
    if (file) file.close();

    int gif = rand() % gifs_vec.size();
    String filename = gifs_vec[gif];
    Serial.println(filename);

    file = LittleFS.open(filename, "r");
    if (!file) {
      Serial.println(": Error opening GIF file");
      while (1) { delay(1000); }; // while 1 loop only triggers watchdog on ESP chips
    }
    Serial.println(": Opened GIF file, start decoding");
    decoder.startDecoding();
  };
  void loop() {
    
  }
  void react(ToggleEvent const &) override { 
    matrix->clear();
    transit<Clock>(); 
  };
  void react(LoopEvent const &) override {
    decoder.loop();
  };
};
FSM_INITIAL_STATE(Switch, Clock)
using fsm_handle = Switch;

ToggleEvent toggle;
LoopEvent loopUpdate;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(74880);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);

  matrix_setup();
  matrix->clear();

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

  // stdout to serial setup
  hal_printf_init();

  // When SD card should be necessary, uncomment this
  // Serial.print("Initializing SD card... ");
  // if (!sd.begin(SD_CS_PIN)) {
  //   error("initialization failed!");
  //   return;
  // }
  // Serial.println("ok");

  // State machine
  // instantiate events
  fsm_handle::start();

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

  //TODO Move in loop and show image during connection
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  Serial.println(WiFi.localIP());

  // Time
  const char* tzConf = configuration["timezone"];
  Serial.println("Timezone");
  Serial.println(tzConf);
  waitForSync();
  // tz.setLocation(tzConf);
  // Serial.println(tz.dateTime());
  setup_webserver();

  decoder.setScreenClearCallback(screenClearCallback);
  decoder.setUpdateScreenCallback(updateScreenCallback);
  decoder.setDrawPixelCallback(drawPixelCallback);
  decoder.setFileSeekCallback(fileSeekCallback);
  decoder.setFilePositionCallback(filePositionCallback);
  decoder.setFileReadCallback(fileReadCallback);
  decoder.setFileReadBlockCallback(fileReadBlockCallback);
}

unsigned long _timer = millis();
void loop() {
  if ((millis() - _timer) >= 10*1000) {
    fsm_handle::dispatch(toggle);
    _timer = millis();
  };

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
