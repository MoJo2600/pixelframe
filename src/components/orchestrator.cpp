#include "components/orchestrator.hpp"
#include "frames/frame.hpp"
#include "frames/clockframe.hpp"
#include "frames/gifframe.hpp"

void Orchestrator::setup(void) {
  // this->currentFrame = new ClockFrame();
  this->currentFrame = new GifFrame();
  this->currentFrame->enter();
}

void Orchestrator::loop(void) {
  this->currentFrame->loop();
}

void Orchestrator::react(FrameEvent e) {

}
