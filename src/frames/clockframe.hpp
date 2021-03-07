#pragma once

#include "frames/frame.hpp"
#include "PongClock.h"

class ClockFrameEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
};

class ClockFrame : public Frame {
  public:
    void loop(void);
    void enter(void);
    // void react(ClockFrameEvent event);
    void exit(void);

  private:
    PongClockClass* clock;

};
