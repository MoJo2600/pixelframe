#ifndef PIXELFRAME_HPP_INCLUDED
#define PIXELFRAME_HPP_INCLUDED

#include <tinyfsm.hpp>
#include <PongClock.h>
#include <FastLED_NeoMatrix.h>

struct ToggleEvent : tinyfsm::Event { };
struct LoopEvent : tinyfsm::Event { };

class PixelframeStateMachine
: public tinyfsm::Fsm<PixelframeStateMachine>
{
  friend class tinyfsm::Fsm<PixelframeStateMachine>;
  friend class tinyfsm::Fsm<PongClockClass>;

  public:
    static FastLED_NeoMatrix * pixel_matrix;
    void PixelframeState(FastLED_NeoMatrix *);
    virtual void react(ToggleEvent const &);
    virtual void react(LoopEvent const &);
    virtual void entry(void) { };  /* entry actions in some states */
    void         exit(void)  { };  /* no exit actions */

    static FastLED_NeoMatrix * getMatrix() { return pixel_matrix; }
};

#endif
