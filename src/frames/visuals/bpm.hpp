#ifndef BPM_HPP_INCLUDED
#define BPM_HPP_INCLUDED

#include "visual.hpp"

class Bpm
: public Visual {
  public:
    void loop(void);
    void enter(void);
    ~Bpm();
};
#endif // BPM_HPP_INCLUDED