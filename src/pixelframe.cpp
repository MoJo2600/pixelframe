#include <tinyfsm.hpp>
#include <pixelframe.hpp>
#include <iostream>
#include <PongClock.h>
#include "lib/gif/GifDecoder.h"

using namespace std;

class ClockState
: public PixelframeStateMachine
{
  using base = PixelframeStateMachine;

  int counter;
  PongClockClass * clock;

  public:
    void entry() override {
        cout << "Entering Clock mode" << endl;
        clock = new PongClockClass(base::pixel_matrix, "Europe/Berlin");
        clock->setup();
    }

    void react(ToggleEvent const &) override {
        cout << "Clock: react" << endl;
    }

    void react(LoopEvent const &) override { 
        clock->loop();
    };

    void exit() {
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

// class PixelframeGifState : PixelframeState
// {
//   private:
//     int OFFSETX = 0;
//     int OFFSETY = 0;
//     const char *pathname = "/gifs/bird.gif";
//     fs::File file;
//     GifDecoder<kMatrixWidth, kMatrixHeight, 10> decoder;
//     bool fileSeekCallback(unsigned long position) { return file.seek(position); }
//     unsigned long filePositionCallback(void) { return file.position(); }
//     int fileReadCallback(void) { return file.read(); }
//     int fileReadBlockCallback(void * buffer, int numberOfBytes) { return file.read((uint8_t*)buffer, numberOfBytes); }
//     void screenClearCallback(void) {}
//     void updateScreenCallback(void) {}
//     void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
//       CRGB color = CRGB(matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
//       matrix->drawPixel(x+OFFSETX, y+OFFSETY, color);
//     }

//   public:
//     PixelframeGifState() {
//       decoder.setScreenClearCallback(screenClearCallback);
//       decoder.setUpdateScreenCallback(updateScreenCallback);
//       decoder.setDrawPixelCallback(drawPixelCallback);
//       decoder.setFileSeekCallback(fileSeekCallback);
//       decoder.setFilePositionCallback(filePositionCallback);
//       decoder.setFileReadCallback(fileReadCallback);
//       decoder.setFileReadBlockCallback(fileReadBlockCallback);
//     }

//     ~PixelframeGifState() {
//       delete decoder;
//     }

// }

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
//     decoder.startDecoding();
//   };
//   void react(ToggleEvent const &) override { 
//     matrix->clear();
//     transit<Clock>(); 
//   };
//   void react(LoopEvent const &) override {
//     decoder.loop();
//   };
// };

// ----------------------------------------------------------------------------
// Base state: default implementations
//
void PixelframeStateMachine::react(ToggleEvent const &) {
  cout << "ToggleEvent event ignored" << endl;
}

void PixelframeStateMachine::react(LoopEvent const &) {
  cout << "LoopEvent event ignored" << endl;
}

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(PixelframeStateMachine, ClockState)
