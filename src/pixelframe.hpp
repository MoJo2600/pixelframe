#pragma once

#include "lib/gif/GifDecoder.h"
#include <tinyfsm.hpp>
#include <PongClock.h>
#include <LittleFS.h>
#include <FastLED_NeoMatrix.h>
#include "ezTime.h"

struct ToggleEvent : tinyfsm::Event { };
struct LoopEvent : tinyfsm::Event { };

class PixelframeStateMachine
: public tinyfsm::Fsm<PixelframeStateMachine>
{
  // friend class tinyfsm::Fsm<PixelframeStateMachine>;
  // friend class tinyfsm::Fsm<PongClockClass>;
  protected:
    static Timezone * tz;

  public:
    static FastLED_NeoMatrix * pixel_matrix;
    void PixelframeState(FastLED_NeoMatrix *) {
      waitForSync();
      tz->setLocation("Europe/Berlin");
    };
    virtual void react(ToggleEvent const &);
    virtual void react(LoopEvent const &);
    virtual void entry(void) { };  /* entry actions in some states */
    void         exit(void)  {
      delete tz;
    };  /* no exit actions */

    static FastLED_NeoMatrix * getMatrix() { return pixel_matrix; }
};
