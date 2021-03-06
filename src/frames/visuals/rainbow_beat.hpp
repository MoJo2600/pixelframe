#ifndef RAINBOW_BEAT_HPP_INCLUDED
#define RAINBOW_BEAT_HPP_INCLUDED

#include "visual.hpp"

class RainbowBeat
: public Visual {
  public:
    void loop(void);
    void enter(void);
  private:
    static RegisterVisual<RainbowBeat> reg;
};
#endif // RAINBOW_BEAT_HPP_INCLUDED