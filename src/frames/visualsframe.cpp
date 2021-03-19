#include <FastLED_NeoMatrix.h>
#include "config.hpp"
#include "frames/frame.hpp"
#include "frames/visualsframe.hpp"
#include "frames/visuals/twinkle.hpp"
#include "frames/visuals/bpm.hpp"
#include "frames/visuals/confetti.hpp"
#include "frames/visuals/rainbow_beat.hpp"
#include "frames/visuals/pacifica.hpp"

#define NUM_PATTERNS 5
#define PATTERN_DURATION 15
uint8_t gCurrentPatternNumber = 1; // Index number of which pattern is current

Frame* VisualsFrameEvent::getFrame() {
  return new VisualsFrame();
}

std::string VisualsFrameEvent::getEventId(void) {
  return "frame.event.visuals.default";
}

void VisualsFrame::nextPattern(void)
{
  delete currentPattern;
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % NUM_PATTERNS;
  switch(gCurrentPatternNumber) {
    case 0:
      currentPattern = new Pacifica();
      break;
    case 1:
      currentPattern = new Bpm();
      break;
    case 2:
      currentPattern = new Confetti();
      break;
    case 3:
      currentPattern = new RainbowBeat();
      break;
    case 4:
      currentPattern = new Twinkle();
      break;
    default:
      currentPattern = new Twinkle();
      break;
  }
}

void VisualsFrame::enter() {
  nextPattern();
}

void VisualsFrame::loop()
{
  if (currentPattern != nullptr) {
    currentPattern->loop();
  }
  EVERY_N_SECONDS( PATTERN_DURATION ) { nextPattern(); } // change patterns periodically
}

void VisualsFrame::react(FrameEvent* event) {
}

void VisualsFrame::exit() {}
