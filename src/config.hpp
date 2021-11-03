#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
/***************************************************************************************
*    Title: <title of program/source code>
*    Author: <author(s) names>
*    Date: <date>
*    Code version: <code version>
*    Availability: <where it's located>
*
***************************************************************************************/

//
// MATRIX
//

// #define FS_NO_GLOBALS // otherwise there is a conflict between fs::File and SD File
// #include <FS.h>
#include <Arduino.h>
#define FS_NO_GLOBALS
#include "LITTLEFS.h"
#include <FastLED.h>

#include <Adafruit_GFX.h>
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

// LED setup
extern uint8_t matrix_brightness;

// Wifi setup
extern char* wifi_ssid;
extern char* wifi_password;

// Choose one of:
#define LED_CHIP WS2812B
#define LED_ORDER GRB
#define LED_DATA_PIN 23

// #define LED_CHIP APA102
// #define LED_ORDER BGR
// #define LED_DATA_PIN D7
// #define LED_CLK_PIN D5

// Used by LEDMatrix
const uint16_t MATRIX_TILE_WIDTH  = 16; // width of EACH NEOPIXEL MATRIX (not total display)
const uint16_t MATRIX_TILE_HEIGHT = 16; // height of each matrix
const uint8_t MATRIX_TILE_H = 1; // number of matrices arranged horizontally
const uint8_t MATRIX_TILE_V = 1; // number of matrices arranged vertically

// Used by NeoMatrix
const uint16_t mw = MATRIX_TILE_WIDTH *  MATRIX_TILE_H;
const uint16_t mh = MATRIX_TILE_HEIGHT * MATRIX_TILE_V;

extern CRGB* matrixleds;

extern FastLED_NeoMatrix * matrix;

// Compat for some other demos
const uint32_t NUMMATRIX = mw*mh;
const uint32_t NUM_LEDS = NUMMATRIX;
const uint16_t MATRIX_HEIGHT = mh;
const uint16_t MATRIX_WIDTH = mw;

// Compat with SmartMatrix code that uses those variables
// (but don't redefine for SmartMatrix backend)
#ifndef SMARTMATRIX
const uint16_t kMatrixWidth = mw;
const uint16_t kMatrixHeight = mh;
#endif

#if defined(ESP8266)
#include "ESP8266WiFi.h"
extern "C" {
#include "user_interface.h"
}
#endif

extern float matrix_gamma; // higher number is darker, needed for Neomatrix more than SmartMatrix

// Like XY, but for a mirror image from the top (used by misconfigured code)
int XY2( int x, int y, bool wrap=false);

uint16_t XY( uint8_t x, uint8_t y);

int wrapX(int x);

void show_free_mem(const char *pre=NULL);

void die(const char *mesg);

void *mallocordie(const char *varname, uint32_t req, bool psram=true);

void matrix_setup(bool initserial=true, int reservemem = 40000);

void set_brightness(uint8_t brightness);

void set_wifi(char* ssid, char* password);

//
// TIMEZONE
//
#include <Arduino.h>
#include "ezTime.h"

extern Timezone* timezone;

//
// ORCHESTRATOR
//
#include <string>

const std::string MODE_CLOCK = "clock";
const std::string MODE_GIF = "randomGif";
const std::string MODE_VISUAL = "randomVisual";
const std::string MODE_OFF = "off";

extern char* default_mode;

void set_default_mode(char* mode);

#endif // CONFIG_H_INCLUDED
// vim:sts=4:sw=4:et
