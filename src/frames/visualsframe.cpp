#include <FastLED_NeoMatrix.h>
#include <iostream>
#include "config.hpp"
#include "frames/frame.hpp"
#include "frames/visualsframe.hpp"
#include <cstring>
#include <string>

#define NUM_PATTERNS 5
#define PATTERN_DURATION 3 // TODO: make configurable

bool randomVisual = false;

Frame* VisualsFrameEvent::getFrame() {
  return new VisualsFrame(this->visual);
}

std::string VisualsFrameEvent::getEventId(void) {
  return "frame.event.visuals.default";
}

VisualsFrame::VisualsFrame(string const& visual) {
  if (visual.c_str() == nullptr || std::strcmp(visual.c_str(), "random") == 0) {
    std::cout << "Random visuals" << std::endl;
    randomVisual = true;
    this->currentPattern = VisualFactory::getRandomVisual();
  } else {
    std::cout << "Selecting visual " << visual << std::endl;
    randomVisual = false;
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
  if (randomVisual) {
    EVERY_N_MINUTES( PATTERN_DURATION ) { nextPattern(); } // change patterns periodically
  }
}

void VisualsFrame::react(FrameEvent* event) {}

void VisualsFrame::exit() {
  delete this->currentPattern;
}
