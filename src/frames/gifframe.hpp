#ifndef GIFFRAME_HPP_INCLUDED
#define GIFFRAME_HPP_INCLUDED

#include <string>
// #include <LittleFS.h>
#include "frames/frame.hpp"
#include "lib/gif/GifDecoder.h"

class RandomGifFrameEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
    std::string getEventId(void);
};

class SingleGifFrameEvent : public FrameEvent {
  public:
    Frame* getFrame(void);
    std::string getEventId(void);
    std::string filename;
    SingleGifFrameEvent(int duration);
};

class GifFrame : public Frame {
  public:
    void loop(void);
    void enter(void);
    void react(FrameEvent* event);
    void exit(void);

  private:
    void playGif(std::string filename);
    bool loadGifToBuffer(std::string filename);
    void freeBuffer(void);

    // RAM buffer for GIF data (eliminates blocking file I/O)
    static uint8_t* gifBuffer;
    static size_t gifBufferSize;
    static size_t gifBufferPos;

    static GifDecoder<16, 16, 10> * decoder;
    
    // Buffer-based callbacks (non-blocking)
    static unsigned long filePositionCallback(void);
    static int fileReadCallback(void);
    static int fileReadBlockCallback(void * buffer, int numberOfBytes);
    static void screenClearCallback(void);
    static void updateScreenCallback(void);
    static void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue);
    static bool fileSeekCallback(unsigned long position);

    FrameEvent* currentEvent;
};

#endif // GIFFRAME_HPP_INCLUDED