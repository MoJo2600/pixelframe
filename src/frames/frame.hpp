#pragma once

#include <string>

class FrameEvent;

class Frame {
  public:
    virtual void loop(void) = 0;
    virtual void enter(void) = 0;
    virtual void react(FrameEvent* event) = 0;
    virtual void exit(void) = 0;
};

class FrameEvent {
  public:
    virtual Frame* getFrame(void) = 0;
    virtual std::string getEventId(void) = 0;
};
