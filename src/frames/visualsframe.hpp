#pragma once

#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include "frames/frame.hpp"

#define SECONDS_PER_PALETTE 10

class VisualsFrame
: public Frame 
{
  public:
    VisualsFrame(CRGB* matrixleds);
    void loop(void);
    void enter(void);
    void exit(void);

  private:
    void drawTwinkles(void);
    void coolLikeIncandescent( CRGB& c, uint8_t phase);
    uint8_t attackDecayWave8( uint8_t i);
    CRGB computeOneTwinkle( uint32_t ms, uint8_t salt);
    void chooseNextColorPalette( CRGBPalette16& pal);
};