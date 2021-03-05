#pragma once

#include "frames/frame.hpp"

class Orchestrator
{
  public:
    void setup(void);
    void loop(void);
    void react(FrameEvent e);

  private:
    Frame* currentFrame;
};
