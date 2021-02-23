#pragma once

#include "lib/gif/GifDecoder.h"
#include <tinyfsm.hpp>
#include <PongClock.h>
#include <LittleFS.h>
#include <FastLED_NeoMatrix.h>
#include "ezTime.h"
#include <iostream>

using namespace std;

struct ToggleEvent : tinyfsm::Event { };
struct LoopEvent : tinyfsm::Event { };

class PixelframeStateMachine
: public tinyfsm::Fsm<PixelframeStateMachine>
{
  // friend class tinyfsm::Fsm<PixelframeStateMachine>;
  // friend class tinyfsm::Fsm<PongClockClass>;

  public:
    static FastLED_NeoMatrix * pixel_matrix;
    static Timezone * timezone;
    void PixelframeState(FastLED_NeoMatrix *, Timezone *);
    virtual void react(ToggleEvent const &);
    virtual void react(LoopEvent const &);
    virtual void entry(void) { };  /* entry actions in some states */
    virtual void exit(void)  {
      cout << "Exit event ignored" << endl;
    };  /* no exit actions */

    static FastLED_NeoMatrix * getMatrix() { return pixel_matrix; }
};
