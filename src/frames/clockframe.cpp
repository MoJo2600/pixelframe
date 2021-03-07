#pragma once

#include <iostream>
#include "frames/clockframe.hpp"
#include "PongClock.h"

using namespace std;

Frame* ClockFrameEvent::getFrame() {
  return new ClockFrame();
}

void ClockFrame::loop(void) {
  if (this->clock) {
    this->clock->loop();
  }
}

void ClockFrame::enter(void) {
  cout << "[PIXELFRAME] Entering Clock mode" << endl;
  this->clock = new PongClockClass(Frame::pixelMatrix, Frame::timezone);
  this->clock->setup();
}

void ClockFrame::exit(void) {
  cout << "[PIXELFRAME] Exit Clock mode" << endl;
  delete this->clock;
}

