#ifndef RAINBOW_BEAT_HPP_INCLUDED
#define RAINBOW_BEAT_HPP_INCLUDED

#include "visual.hpp"

class RainbowBeat
: public Visual {
  public:
    void loop(void);
    void enter(void);
    ~RainbowBeat(void);
  private:
    CRGB matrixleds_buf[NUM_LEDS];
};
#endif // RAINBOW_BEAT_HPP_INCLUDED