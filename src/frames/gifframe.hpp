#pragma once

#include <LittleFS.h>
#include "frames/frame.hpp"
#include "lib/gif/GifDecoder.h"

class GifFrameEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
};

class GifFrame : public Frame {
  public:
    void loop(void);
    void enter(void);
    // void react(ClockFrameEvent event);
    void exit(void);

  private:
    static fs::File file;
    static GifDecoder<16, 16, 10> * decoder;
    static unsigned long filePositionCallback(void);
    static int fileReadCallback(void);
    static int fileReadBlockCallback(void * buffer, int numberOfBytes);
    static void screenClearCallback(void);
    static void updateScreenCallback(void);
    static void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue);
    static bool fileSeekCallback(unsigned long position);

};
