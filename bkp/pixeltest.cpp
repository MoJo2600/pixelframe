/* rainbow_beat
 *
 * By: Andrew Tuline
 *
 * Date: July, 2015
 * 
 * A rainbow moving back and forth using FastLED function calls with no delays or 'for' loops. Very simple, yet effective.
 *
 */
#include <iostream>
#include <FastLED_NeoMatrix.h>
#include "config.hpp"
#include "frames/visuals/rainbow_beat.hpp"

using namespace std;

void PixelTest::enter() {}

void PixelTest::loop()
{
  uint8_t beatA = beatsin8(17, 0, 255);                        // Starting hue
  uint8_t beatB = beatsin8(13, 0, 255);

  CHSV hsv;
  hsv.hue = (beatA+beatB)/2;
  hsv.val = 255;
  hsv.sat = 255;
  for (byte y=0; y<MATRIX_TILE_WIDTH; y++) {

    for (byte x=0; x<MATRIX_TILE_HEIGHT; x++) {
      matrixleds[XY(x,y)] = hsv;
    }
 
    hsv.hue += 8;
  }
}
