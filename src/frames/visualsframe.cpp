#include <FastLED_NeoMatrix.h>
#include <iostream>
#include "config.hpp"
#include "frames/frame.hpp"
#include "frames/visualsframe.hpp"
// #include "frames/visuals/twinkle.hpp"
// #include "frames/visuals/bpm.hpp"
// #include "frames/visuals/confetti.hpp"
// #include "frames/visuals/rainbow_beat.hpp"
// #include "frames/visuals/pacifica.hpp"

#define NUM_PATTERNS 5
#define PATTERN_DURATION 45 // TODO: make configurable
uint8_t gCurrentPatternNumber = 1; // Index number of which pattern is current

using namespace std;

Frame* VisualsFrameEvent::getFrame() {
  return new VisualsFrame(this->visual);
}

std::string VisualsFrameEvent::getEventId(void) {
  return "frame.event.visuals.default";
}

VisualsFrame::VisualsFrame(string const& visual) {
  this->currentPattern = VisualFactory::createInstance(visual);
  std::cout << "[FRAME::VISUALS] Constructor pattern: " << this->currentPattern << std::endl;
}

void VisualsFrame::nextPattern(void)
{
  // delete currentPattern;
  // add one to the current pattern number, and wrap around at the end
  // gCurrentPatternNumber = (gCurrentPatternNumber + 1) % NUM_PATTERNS;
  // std::cout << "[FRAME::VISUALS] next pattern: " << gCurrentPatternNumber << std::endl;
  // switch(gCurrentPatternNumber) {
  //   case 0:
  //     cout << "[Visualsframe] Pacifica" << endl;
  //     currentPattern = new Pacifica();
  //     break;
  //   case 1:
  //     cout << "[Visualsframe] Bpm" << endl;
  //     currentPattern = new Bpm();
  //     break;
  //   case 2:
  //     cout << "[Visualsframe] Confetti" << endl;
  //     currentPattern = new Confetti();
  //     break;
  //   case 3:
  //     cout << "[Visualsframe] Rainbow" << endl;
  //     currentPattern = new RainbowBeat();
  //     break;
  //   case 4:
  //     cout << "[Visualsframe] Twinkle" << endl;
  //     currentPattern = new Twinkle();
  //     break;
  // }
}

void VisualsFrame::enter() {
  if (currentPattern != nullptr) {
    currentPattern->enter();
  }
  // nextPattern();
}

void VisualsFrame::loop()
{
  if (currentPattern != nullptr) {
    currentPattern->loop();
  }
  // EVERY_N_SECONDS( PATTERN_DURATION ) { nextPattern(); } // change patterns periodically
}

void VisualsFrame::react(FrameEvent* event) {
}

void VisualsFrame::exit() {
  delete currentPattern;
}
