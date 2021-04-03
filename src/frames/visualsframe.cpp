#include <FastLED_NeoMatrix.h>
#include <iostream>
#include "config.hpp"
#include "frames/frame.hpp"
#include "frames/visualsframe.hpp"

#define NUM_PATTERNS 5
#define PATTERN_DURATION 3 // TODO: make configurable

using namespace std;

Frame* VisualsFrameEvent::getFrame() {
  return new VisualsFrame(this->visual);
}

std::string VisualsFrameEvent::getEventId(void) {
  return "frame.event.visuals.default";
}

VisualsFrame::VisualsFrame(string const& visual) {
  if (visual == "random") {
    this->random = true;
    this->currentPattern = VisualFactory::getRandomVisual();
  } else {
    this->currentPattern = VisualFactory::createInstance(visual);
  }
}

void VisualsFrame::nextPattern(void)
{
  if (this->currentPattern != nullptr) {
    delete this->currentPattern;
  }
  this->currentPattern = VisualFactory::getRandomVisual();
}

void VisualsFrame::enter() {
  if (currentPattern != nullptr) {
    currentPattern->enter();
  }
}

void VisualsFrame::loop()
{
  if (currentPattern != nullptr) {
    currentPattern->loop();
  }
  if (this->random) {
    EVERY_N_MINUTES( PATTERN_DURATION ) { nextPattern(); } // change patterns periodically
  }
}

void VisualsFrame::react(FrameEvent* event) {}

void VisualsFrame::exit() {
  delete currentPattern;
}
