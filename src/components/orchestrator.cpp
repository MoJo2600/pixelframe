#include <iostream>
#include "components/orchestrator.hpp"
#include "frames/frame.hpp"
#include "frames/clockframe.hpp"
#include "frames/gifframe.hpp"

Orchestrator* Orchestrator::instance = nullptr;

void Orchestrator::setup(void) {
  this->currentFrame = nullptr;

  // TODO: define default behavior

  // auto ev = new RandomGifFrameEvent();
  auto ev = new ClockFrameEvent();

  this->react(ev);
}

void Orchestrator::loop(void) {
  this->currentFrame->loop();
}

void Orchestrator::react(FrameEvent* e) {
  std::cout << "[COMPONENT::ORCHESTRATOR] Reacting" << std::endl;

  if (this->currentFrame != nullptr) {
    std::cout << "[COMPONENT::ORCHESTRATOR] Cleaning up old frame" << std::endl;
    
    this->currentFrame->exit();

    delete this->currentFrame;
  }

  std::cout << "[COMPONENT::ORCHESTRATOR] Initializing new frame" << std::endl;

  this->currentFrame = e->getFrame();

  std::cout << "[COMPONENT::ORCHESTRATOR] Entering and reacting on new frame" << std::endl;

  this->currentFrame->enter();
  this->currentFrame->react(e);
}
