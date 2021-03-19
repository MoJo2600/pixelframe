#include <iostream>
#include "components/orchestrator.hpp"
#include "frames/frame.hpp"
#include "frames/clockframe.hpp"
#include "config.hpp"
#include "frames/gifframe.hpp"
#include "frames/visualsframe.hpp"

Orchestrator* Orchestrator::instance = nullptr;

unsigned long fadeStartTime = 0;
unsigned int fadeLength = 500;

void Orchestrator::setup() {
  this->currentFrame = nullptr;
  this->lastEvent = nullptr;
  this->currentEvent = nullptr;
  this->nextEvent = nullptr;

  //auto ev = new VisualsFrameEvent();
  auto ev = new ClockFrameEvent();
  // auto ev = new RandomGifFrameEvent();
  this->react(ev);
}

void Orchestrator::loop(void) {
  // Fade out and switch after fade
  if (this->currentFrame != nullptr) {

    this->currentFrame->loop();

    // Fade in
    if (fadeStartTime + fadeLength > millis())
    {
      uint8_t fadeAmount = map(millis(), fadeStartTime, fadeStartTime + fadeLength, 0, matrix_brightness);
      FastLED.setBrightness(fadeAmount);
    }
  }
  if (this->nextEvent != nullptr) {
    // Fade out
    if (fadeStartTime + fadeLength > millis())
    {
      uint8_t fadeAmount = map(millis(), fadeStartTime, fadeStartTime + fadeLength, matrix_brightness, 0);
      FastLED.setBrightness(fadeAmount);
    } else {
      this->switchFrame(this->nextEvent);
      fadeStartTime = millis(); // fade in
    }
  }

  matrix->show();
  // EVERY_N_MILLISECONDS(12) { matrix->show(); }; // Limit to 80fps
}

void Orchestrator::react(FrameEvent* e) {
  std::cout << "[COMPONENT::ORCHESTRATOR] Reacting" << std::endl;
  fadeStartTime = millis(); // fade out
  this->nextEvent = e;
}

void Orchestrator::switchFrame(FrameEvent* e) {
  std::cout << "[COMPONENT::ORCHESTRATOR] Switching" << std::endl;

  this->nextEvent = nullptr;

  if (this->currentFrame != nullptr) {
    std::cout << "[COMPONENT::ORCHESTRATOR] Cleaning up old frame" << std::endl;
    
    this->currentFrame->exit();

    delete this->currentFrame;

    std::cout << "[COMPONENT::ORCHESTRATOR] Memory after state switch" << std::endl;
    show_free_mem();
  }

  std::cout << "[COMPONENT::ORCHESTRATOR] Initializing new frame" << std::endl;

  this->currentFrame = e->getFrame();

  std::cout << "[COMPONENT::ORCHESTRATOR] Entering and reacting on new frame" << std::endl;

  this->currentFrame->enter();
  this->currentFrame->react(e);
}
