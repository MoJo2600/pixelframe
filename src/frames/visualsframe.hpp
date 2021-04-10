#ifndef VISUALSFRAME_HPP_INCLUDED
#define VISUALSFRAME_HPP_INCLUDED

#include <string.h>
#include <FastLED_NeoMatrix.h>
#include "frames/frame.hpp"
#include "frames/visuals/visual.hpp"
// #include "frames/visuals/twinkle.hpp"
#include <map>

using std::string;

class VisualsFrameEvent : public FrameEvent
{
public:
  Frame *getFrame(void);
  string getEventId(void);
  string visual;
};

class VisualsFrame
    : public Frame
{
public:
  void loop(void);
  void enter(void);
  void react(FrameEvent *event);
  void exit(void);
  VisualsFrame(string const& visual);
private:
  Visual *currentPattern;
  void nextPattern(void);
};

#endif // VISUALSFRAME_HPP_INCLUDED