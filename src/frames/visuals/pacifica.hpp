#ifndef PACIFICA_HPP_INCLUDED
#define PACIFICA_HPP_INCLUDED
//
//  "Pacifica"
//  Gentle, blue-green ocean waves.
//  December 2019, Mark Kriegsman and Mary Corey March.
//  For Dan.
//
#include <FastLED_NeoMatrix.h>
#include "visual.hpp"

//////////////////////////////////////////////////////////////////////////
//
// The code for this animation is more complicated than other examples, and 
// while it is "ready to run", and documented in general, it is probably not 
// the best starting point for learning.  Nevertheless, it does illustrate some
// useful techniques.
//
//////////////////////////////////////////////////////////////////////////
//
// In this animation, there are four "layers" of waves of light.  
//
// Each layer moves independently, and each is scaled separately.
//
// All four wave layers are added together on top of each other, and then 
// another filter is applied that adds "whitecaps" of brightness where the 
// waves line up with each other more.  Finally, another pass is taken
// over the led array to 'deepen' (dim) the blues and greens.
//
// The speed and scale and motion each layer varies slowly within independent 
// hand-chosen ranges, which is why the code has a lot of low-speed 'beatsin8' functions
// with a lot of oddly specific numeric ranges.
//
// These three custom blue-green color palettes were inspired by the colors found in
// the waters off the southern coast of California, https://goo.gl/maps/QQgd97jjHesHZVxQ7
//

class Pacifica
: public Visual {
  public:
    void loop(void);
    void enter(void);

  private:
    void pacifica_one_layer(CRGBPalette16 p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff);
    void pacifica_add_whitecaps();
    void pacifica_deepen_colors();
    void pacifica_loop();
    static RegisterVisual<Pacifica> reg;
};
#endif // PACIFICA_HPP_INCLUDED