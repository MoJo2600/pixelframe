#include <iostream>
#include "components/orchestrator.hpp"
#include "frames/frame.hpp"
#include "frames/clockframe.hpp"
#include "config.hpp"
#include "frames/gifframe.hpp"
#include "frames/visualsframe.hpp"
#include "frames/off.hpp"

#define FADE_LENGTH 500

Orchestrator* Orchestrator::instance = nullptr;

unsigned long fadeStartTime = 0;

void Orchestrator::setup() {
  this->currentFrame = nullptr;
  this->lastEvent = nullptr;
  this->currentEvent = nullptr;
  this->nextEvent = nullptr;

  auto ev = new ClockFrameEvent();

  this->react(ev);
}

void Orchestrator::loop(void) {
  // If the event has a duration, switch back to the last event after the time has passed
  if (this->currentEvent != nullptr &&
      this->currentEvent->duration > 0 && 
      millis() - this->frameActiveSince > this->currentEvent->duration * 1000u) {
    std::cout << "[COMPONENT::ORCHESTRATOR] Event timeout, switching back to last event" << std::endl;
    this->switchFrame(this->lastEvent);
  }

  // Fade out and switch after fade
  if (this->currentFrame != nullptr) {

    this->currentFrame->loop();

    // Fade in
    if (fadeStartTime + FADE_LENGTH > millis())
    {
      uint8_t fadeAmount = map(millis(), fadeStartTime, fadeStartTime + FADE_LENGTH, 0, matrix_brightness);
      FastLED.setBrightness(fadeAmount);
    }
  }
  if (this->nextEvent != nullptr) {
    // Fade out
    if (fadeStartTime + FADE_LENGTH > millis())
    {
      uint8_t fadeAmount = map(millis(), fadeStartTime, fadeStartTime + FADE_LENGTH, matrix_brightness, 0);
      FastLED.setBrightness(fadeAmount);
    } else {
      this->switchFrame(this->nextEvent);
      fadeStartTime = millis(); // fade in
    }
  }

  EVERY_N_MILLISECONDS(15) { matrix->show(); }; // Limit to ~70fps
}

void Orchestrator::react(FrameEvent* e) {
  std::cout << "[COMPONENT::ORCHESTRATOR] Reacting" << std::endl;
  fadeStartTime = millis(); // fade out
  this->lastEvent = this->currentEvent;
  this->nextEvent = e;
}

void Orchestrator::switchFrame(FrameEvent* e) {
  std::cout << "[COMPONENT::ORCHESTRATOR] Switching" << std::endl;

  this->lastEvent = this->currentEvent;
  this->currentEvent = e;
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
  this->frameActiveSince = millis();
}
