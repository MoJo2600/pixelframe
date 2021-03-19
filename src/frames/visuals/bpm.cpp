#include <FastLED_NeoMatrix.h>
#include "config.hpp"
#include "frames/visuals/bpm.hpp"

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

Bpm::~Bpm() {}

void Bpm::enter() {}

void Bpm::loop()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  int led = 0;
  for (byte y=0; y<MATRIX_TILE_HEIGHT; y++) {
    for (byte x=0; x<MATRIX_TILE_WIDTH; x++) {
      matrixleds[XY(x,y)] = ColorFromPalette(palette, gHue+(led*2), beat-gHue+(led*10));
      led++;
    }
  }
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}
