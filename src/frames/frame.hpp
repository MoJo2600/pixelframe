#pragma once

#include <string>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <FastLED_NeoMatrix.h>
#include "ezTime.h"

class FrameEvent;

class Frame {
  public:
    virtual void loop(void) = 0;
    virtual void enter(void) = 0;
    virtual void react(FrameEvent* event) = 0;
    virtual void exit(void) = 0;

    static FastLED_NeoMatrix* pixelMatrix;
    static Timezone* timezone;
};

class FrameEvent {
  public:
    virtual Frame* getFrame(void) = 0;
    virtual std::string getEventId(void) = 0;
};
