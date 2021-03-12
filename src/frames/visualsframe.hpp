#pragma once

#include <string.h>
#include <FastLED_NeoMatrix.h>
#include "frames/frame.hpp"

#define SECONDS_PER_PALETTE 10

class VisualsFrameEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
    std::string getEventId(void);
};

class VisualsFrame
: public Frame 
{
  public:
    void loop(void);
    void enter(void);
    void react(FrameEvent* event);
    void exit(void);

  private:
    void drawTwinkles(void);
    void coolLikeIncandescent( CRGB& c, uint8_t phase);
    uint8_t attackDecayWave8( uint8_t i);
    CRGB computeOneTwinkle( uint32_t ms, uint8_t salt);
    void chooseNextColorPalette( CRGBPalette16& pal);
};