#include <tinyfsm.hpp>
#include <pixelframe.hpp>
#include <iostream>
#include <PongClock.h>
#define FS_NO_GLOBALS
#include <LittleFS.h>
#include "lib/gif/GifDecoder.h"
#include "ezTime.h"

using namespace std;

class GifDecoderState; // Forward declaration

class ClockState
: public PixelframeStateMachine
{
  using base = PixelframeStateMachine;

  int counter;
  PongClockClass * clock;

  public:
    void entry() override {
        cout << "[PIXELFRAME] Entering Clock mode" << endl;
        clock = new PongClockClass(base::pixel_matrix, base::timezone);
        clock->setup();
    }

    void react(ToggleEvent const &) override {
      cout << "[PIXELFRAME] Clock: react" << endl;
      base::pixel_matrix->clear();
      transit<GifDecoderState>();
    }

    void react(LoopEvent const &) override { 
      clock->loop();
    };

    void exit() override {
      cout << "[PIXELFRAME] Exit Clock mode" << endl;
      delete clock;
    }
};


// class Clock
// : public PixelframeState
// {
//   private:
//     FastLED_NeoMatrix * matrix;
//     PixelFrame::PongClockClass pongClock;

//   void entry() override { 
//     cout << "Entering Clock mode" << endl;
//     // pongClock  = new PixelFrame::PongClockClass(matrix, "Europe/Berlin"); // TODO: Read tz from config
//     // pongClock->setup();
//   };
//   void react(ToggleEvent const &) override {
//     cout << "Clock: react" << endl;
//     // matrix->clear();
//     // transit<Gif>();
//   };
//   void react(LoopEvent const &) override { 
//     // pongClock->loop();
//   };
//   void exit() {
//     // delete pongClock;
//   }
// };

// Base Class for any state that needs to utilize GifDecoder
class GifDecoderState :
public PixelframeStateMachine
{
  using base = PixelframeStateMachine;

  protected:
    // static FS * fileSystem;
    static fs::File file;
    static GifDecoder<16, 16, 10> * decoder;
    const char * filename = "/gifs/city.gif";

  // protected:
    // GifDecoder<16, 16, 10> decoder;
    // const char * filename = "/gifs/bird.gif";

    static unsigned long filePositionCallback(void) { return file.position(); }
    static int fileReadCallback(void) { return file.read(); }
    static int fileReadBlockCallback(void * buffer, int numberOfBytes) { return file.read((uint8_t*)buffer, numberOfBytes); }
    static void screenClearCallback(void) {}
    static void updateScreenCallback(void) {}
    static void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
      CRGB color = CRGB(base::pixel_matrix->gamma[red], base::pixel_matrix->gamma[green], base::pixel_matrix->gamma[blue]);
      base::pixel_matrix->drawPixel(x, y, color);
    }
    static bool fileSeekCallback(unsigned long position) { return file.seek(position); }

  public:
    // 

    void entry() override {
      decoder = new GifDecoder<16, 16, 10>();
      decoder->setFilePositionCallback(filePositionCallback);
      decoder->setFileReadCallback(fileReadCallback);
      decoder->setFileReadBlockCallback(fileReadBlockCallback);
      decoder->setScreenClearCallback(screenClearCallback);
      decoder->setUpdateScreenCallback(updateScreenCallback);
      decoder->setDrawPixelCallback(drawPixelCallback);
      decoder->setFileSeekCallback(fileSeekCallback);

      //TODO: Move to sub states, that will use the gif decoder
      if (file) file.close();
      file = LittleFS.open(filename, "r");
      if (!file) {
        cout << "[PIXELFRAME] Error opening GIF file" << endl;
      }
      cout << "[PIXELFRAME] Opened GIF file, start decoding" << endl;
      decoder->startDecoding();
    }

    void react(ToggleEvent const &) override {
      base::pixel_matrix->clear();
      transit<ClockState>();
    }

    void react(LoopEvent const &) override {
      decoder->loop();
    }

    void exit() override {
      cout << "[PIXELFRAME] Exit Gif mode" << endl;
      decoder->stop();
      if (file) {
        cout << "[PIXELFRAME] Close file" << endl;
        file.close();
      } 
      cout << "[PIXELFRAME] Delete decoder" << endl;
      delete decoder;
    }
};

// FS * GifDecoderState::fileSystem = &LittleFS;
fs::File GifDecoderState::file;
GifDecoder<16, 16, 10> * GifDecoderState::decoder; //  = new GifDecoder<16, 16, 10>()

// class Gif
// : public PixelframeGifState
// {
//   void entry() override { 
//     if (file) file.close();

//     int gif = rand() % gifs_vec.size();
//     String filename = gifs_vec[gif];
//     Serial.println(filename);

//     file = LittleFS.open(filename, "r");
//     if (!file) {
//       Serial.println(": Error opening GIF file");
//       while (1) { delay(1000); }; // while 1 loop only triggers watchdog on ESP chips
//     }
//     Serial.println(": Opened GIF file, start decoding");
//     decoder->startDecoding();
//   };
//   void react(ToggleEvent const &) override { 
//     matrix->clear();
//     transit<Clock>(); 
//   };
//   void react(LoopEvent const &) override {
//     decoder->loop();
//   };
// };

// ----------------------------------------------------------------------------
// Base state: default implementations
//
void PixelframeStateMachine::react(ToggleEvent const &) {
  cout << "[PIXELFRAME] ToggleEvent event ignored" << endl;
}

void PixelframeStateMachine::react(LoopEvent const &) {
  cout << "[PIXELFRAME] LoopEvent event ignored" << endl;
}

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(PixelframeStateMachine, ClockState)
