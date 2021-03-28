#ifndef OFF_HPP_INCLUDED
#define OFF_HPP_INCLUDED

#include <string.h>
#include <FastLED_NeoMatrix.h>
#include "frames/frame.hpp"


class OffEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
    std::string getEventId(void);
};

class Off
: public Frame 
{
  public:
    void loop(void);
    void enter(void);
    void react(FrameEvent* event);
    void exit(void);
};

#endif // OFF_HPP_INCLUDED
