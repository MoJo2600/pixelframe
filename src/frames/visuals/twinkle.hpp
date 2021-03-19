#ifndef TWINKLE_HPP_INCLUDED
#define TWINKLE_HPP_INCLUDED

#include "visual.hpp"

class Twinkle
: public Visual {
  public:
    void loop(void);
    void enter(void);
  private:
    void drawTwinkles(void);
    void coolLikeIncandescent( CRGB& c, uint8_t phase);
    uint8_t attackDecayWave8( uint8_t i);
    CRGB computeOneTwinkle( uint32_t ms, uint8_t salt);
    void chooseNextColorPalette( CRGBPalette16& pal);
};
#endif // TWINKLE_HPP_INCLUDED