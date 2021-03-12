#ifndef FRAME_HPP_INCLUDED
#define FRAME_HPP_INCLUDED

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

#endif // FRAME_HPP_INCLUDED