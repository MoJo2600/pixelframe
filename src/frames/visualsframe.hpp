#ifndef VISUALSFRAME_HPP_INCLUDED
#define VISUALSFRAME_HPP_INCLUDED

#include <string.h>
#include <FastLED_NeoMatrix.h>
#include "frames/frame.hpp"
#include "frames/visuals/visual.hpp"

class VisualsFrameEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
    std::string getEventId(void);
};

class VisualsFrame
: public Frame 
{
  public:
    void loop(void);
    void enter(void);
    void react(FrameEvent* event);
    // void signalExit(void);
    void exit(void);
    // ~VisualsFrame();
  
  private:
    Visual* currentPattern;
    void nextPattern(void);
};

#endif // VISUALSFRAME_HPP_INCLUDED