#ifndef CONFETTI_HPP_INCLUDED
#define CONFETTI_HPP_INCLUDED

#include "visual.hpp"

class Confetti
: public Visual {
  public:
    void loop(void);
    void enter(void);
  private:
    void confetti(void);
    void changeMe(void);

    int thishue = 50,                                       // Starting hue.
        huediff = 256;                                      // Range of random #'s to use for hue

    uint8_t thisfade = 8,                                   // How quickly does it fade? Lower = slower fade rate.
            thisinc = 1,                                    // Incremental value for rotating hues
            thissat = 100,                                  // The saturation, where 255 = brilliant colours.
            thisbri = 255,                                  // Brightness of a sequence. Remember, max_bright is the overall limiter.
            thisdelay = 5;                                  // We don't need much delay (if any)
    static RegisterVisual<Confetti> reg;
};
#endif // CONFETTI_HPP_INCLUDED