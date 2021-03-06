#include <iostream>
#include "frames/clockframe.hpp"
#include "config.hpp"
#include "PongClock.hpp"

using namespace std;

Frame* ClockFrameEvent::getFrame() {
  return new ClockFrame();
}

std::string ClockFrameEvent::getEventId(void) {
  return "frame.event.clock.default";
}

void ClockFrame::loop(void) {
  if (this->clock) {
    this->clock->loop();
  }
}

void ClockFrame::enter(void) {
  cout << "[PIXELFRAME] Entering Clock mode" << endl;
  this->clock = new PongClockClass(timezone);
  this->clock->setup();
}

void ClockFrame::react(FrameEvent* event) {
}

void ClockFrame::exit(void) {
  cout << "[PIXELFRAME] Exit Clock mode" << endl;
  delete this->clock;
}

