#pragma once

#include <string>
#include "frames/frame.hpp"
#include <Arduino.h>
#include "PongClock.h"

class ClockFrameEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
    std::string getEventId(void);
};

class ClockFrame : public Frame {
  public:
    void loop(void);
    void enter(void);
    void react(FrameEvent* event);
    void exit(void);

  private:
    PongClockClass* clock;

};
