#include <iostream>
#include "components/orchestrator.hpp"
#include "frames/frame.hpp"
#include "frames/clockframe.hpp"
#include "frames/gifframe.hpp"

Orchestrator* Orchestrator::instance = nullptr;

void Orchestrator::setup(void) {
  // this->currentFrame = new ClockFrame();
  this->currentFrame = new GifFrame();
  this->currentFrame->enter();
}

void Orchestrator::loop(void) {
  this->currentFrame->loop();
}

void Orchestrator::react(FrameEvent* e) {
  std::cout << "[COMPONENT::ORCHESTRATOR] Reacting" << std::endl;

  if (this->currentFrame) {
    this->currentFrame->exit();
  }

  delete this->currentFrame;

  this->currentFrame = e->getFrame();

  this->currentFrame->enter();
}
