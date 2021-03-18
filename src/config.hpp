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
#define FS_NO_GLOBALS
#include <LittleFS.h>

#include <FastLED.h>

#include <Adafruit_GFX.h>
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include <FastLED_NeoMatrix.h>
#define FASTLED_NEOMATRIX

// LED setup
#define FRAMES_PER_SECOND 60

extern uint8_t matrix_brightness;

// Choose one of:
#define LED_CHIP WS2812B
#define LED_ORDER GRB
#define LED_DATA_PIN D7

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

// MATRIX DECLARATION:
// Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = pin number (most are valid)
// Parameter 6 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
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

#include "ESP8266WiFi.h"
extern "C" {
#include "user_interface.h"
}

extern uint8_t gHue; // rotating "base color" used by many of the patterns
extern uint16_t speed;
extern float matrix_gamma; // higher number is darker, needed for Neomatrix more than SmartMatrix

// Like XY, but for a mirror image from the top (used by misconfigured code)
int XY2( int x, int y, bool wrap=false);

uint16_t XY( uint8_t x, uint8_t y);

int wrapX(int x);

void show_free_mem(const char *pre=NULL);

void die(const char *mesg);

void *mallocordie(const char *varname, uint32_t req, bool psram=true);

void matrix_setup(bool initserial=true, int reservemem = 40000);

//
// TIMEZONE
//

#include <Arduino.h>
#include "ezTime.h"

extern Timezone* timezone;

#endif // CONFIG_H_INCLUDED
// vim:sts=4:sw=4:et
