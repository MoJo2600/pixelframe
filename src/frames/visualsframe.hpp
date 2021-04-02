#ifndef VISUALSFRAME_HPP_INCLUDED
#define VISUALSFRAME_HPP_INCLUDED

#include <string.h>
#include <FastLED_NeoMatrix.h>
#include "frames/frame.hpp"
#include "frames/visuals/visual.hpp"
// #include "frames/visuals/twinkle.hpp"
#include <map>

using std::string;

// enum VisualEffects
// {
//   twinkle = "twinkle"
// };

// struct VisualEffect
// {
//   string name;
//   Visual visual;
// };

// const std::map<std::string, VisualEffect> visualEffectsList = {
//     {"twinkle", {"Twinkle", Twinkle}}
// };

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
  // void signalExit(void);
  void exit(void);
  VisualsFrame(string const& visual);
  // ~VisualsFrame();

private:
  Visual *currentPattern;
  void nextPattern(void);
};

#endif // VISUALSFRAME_HPP_INCLUDED