/*
  PixelFrame

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13

 TODO: LED connection

 created   July 2020
 by Christian Erhardt
 */
// #define _stackSize (6748/4)
#define FS_NO_GLOBALS // otherwise there is a conflict between fs::File and SD File
#include "SPI.h"
#include "lib/stdinout.h"
#include "FastLED.h"
#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "MediaPlayer.h"
#include "SdFat.h"

// SD Card setup
SdFat sd;
// SdFile dirFile;
File fd;

#define SD_CS_PIN 4

// LED setup
#define NUM_LEDS 256
#define DATA_PIN 5
#define BRIGHTNESS  64
CRGB leds[NUM_LEDS];

StaticJsonDocument<512> configuration;

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))


// void play_gif(File* fd) {
//   uint8_t *color, *frame;
//   int ret = 1;
//   unsigned long previousMillis = 0;
//   unsigned long delay = 10;

//   int startIndex = 0;
//   int index = 0;
//   unsigned long currentMillis = millis();
//   int repeat = 0;
//   // while (fd.openNext(&dirFile, O_RDONLY)) {
//   //   if (!fd.isHidden() && !fd.isSubDir()) {


//   //   fd.close();
//   // }

//     gif = gd_open_gif(&fd);
//     delay = gif->gce.delay+1 * 100;

//     if (gif == NULL) {
//       Serial.println("failure reading gif");
//       return;
//     }

//     frame = (uint8_t*) malloc(gif->width * gif->height * 3);
//     repeat = 0;
//     while (repeat <= 3) // play only thrice
//     {
//       currentMillis = millis();
//       if (currentMillis - previousMillis >= delay)
//       {
//         ret = gd_get_frame(gif);
//         if (ret == -1)
//         {
//           Serial.println("Could not load gif frame");
//           return;
//         }

//         gd_render_frame(gif, frame);

//         color = frame;

//         for (int i = 0; i < gif->height; i++)
//         {
//           startIndex = i * 16;
//           index = 0;
//           for (int j = 0; j < gif->width; j++) {
//             if (i % 2 == 0) {
//                 index = startIndex + j;
//             } else {
//                 index = startIndex + 15 - j;
//             }
//             leds[index].setRGB(color[0], color[1], color[2]);
//             color += 3;
//           }
//         }
//         FastLED.show();

//         if (ret == 0) {
//           repeat++;
//           Serial.println(repeat);
//           gd_rewind(gif);
//         }

//         previousMillis = currentMillis;
//       }
//     }
//     free(frame);
//     gd_close_gif(gif);
//   }
// }

// loops
void gif_loop() {

}


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);

  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is assumed
  FastLED.setBrightness(BRIGHTNESS);

  // Black screen
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(0, 0, 255);
  }
  FastLED.show();
  // End Black screen

  // stdout to serial setup
  hal_printf_init();

  Serial.print("Initializing SD card... ");
  if (!sd.begin(SD_CS_PIN)) {
    error("initialization failed!");
    return;
  }
  Serial.println("ok");

  // Show no wifi icon
  // Switch to bmp
  // MediaPlayer.play("system/nowifi.gif");

  // // ### READ CONFIG
  // Serial.print("Opening configuration file... ");
  // if (!fd.open("system/config.json", O_READ))
  // {
  //   error("opening config failed");
  //   return;
  // }
  // Serial.println("ok");

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
  //   Serial.println(F("Failed to read file, using default configuration"));

  // // ### END: READ CONFIG

  // // ### CONNECT WIFI
  // const char* ssid = configuration["wifi"]["ssid"];
  // const char* password = configuration["wifi"]["password"];

  // Serial.println("Clear screen");

  // Serial.println("Connect wifi");

  // WiFi.begin(ssid, password);

  // while ( WiFi.status() != WL_CONNECTED ) {
  //   delay (500);
  //   Serial.print ( "." );
  // }
  // Serial.println("connected");
  // // ### CONNECT WIFI


  MediaPlayer.setup(leds, &sd);
  MediaPlayer.play("bird.gif");

  // while(true) {
  //   leds[0].setRGB(255, 0, 0);
  //   FastLED.show();
  //   delay(1000);
  //   leds[0].setRGB(0, 255, 0);
  //   FastLED.show();
  //   delay(1000);
  //   leds[0].setRGB(0, 0, 255);
  //   FastLED.show();
  //   delay(1000);
  // }

  // if (!dirFile.open("/")) {
  //   error("open root failed");
  // }

}

void loop() {
  //
  MediaPlayer.loop();

  
}
