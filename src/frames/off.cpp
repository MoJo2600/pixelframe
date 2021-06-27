#include <FastLED_NeoMatrix.h>
#include <iostream>
#include "config.hpp"
#include "frames/frame.hpp"
#include "frames/off.hpp"

Frame* OffEvent::getFrame() {
  return new Off();
}

std::string OffEvent::getEventId(void) {
  return "frame.event.visuals.default";
}

void Off::enter() {
  matrix->clear();
}

void Off::loop() {}

void Off::react(FrameEvent* event) {
  matrix->clear();
}

void Off::exit() {}
