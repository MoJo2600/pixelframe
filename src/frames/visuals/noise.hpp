#ifndef NOISE_HPP_INCLUDED
#define NOISE_HPP_INCLUDED
#include <FastLED_NeoMatrix.h>
#include "visual.hpp"
#include "config.hpp"

DECLARE_GRADIENT_PALETTE(pit);

class Noise
: public Visual {
  public:
    void loop(void);
    void enter(void);
  private:
    // matrix size
    // uint8_t Width,
    //         Height,
    //         CentreX,
    //         CentreY,
    //         noise[1][16][16];
    // uint32_t x[],
    //          y[],
    //          z[],
    //          scale_x[],
    //          scale_y[];

    static RegisterVisual<Noise> reg;
    void adjust_gamma(void);
    void make_some_noise(void);
};
#endif // NOISE_HPP_INCLUDED