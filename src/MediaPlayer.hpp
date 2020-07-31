#pragma once

#include "lib/gifdec.h"
#include "FastLED.h"
#include "SdFat.h"

namespace PixelFrame {

    class MediaPlayerClass {
        public:
            MediaPlayerClass();
            ~MediaPlayerClass();
            void setup(CRGB *ledConfig, SdFat *sdFat);
            void play(char *filename);
            void stop();
            void loop();

        private:
            unsigned long previousMillis = 0;
            unsigned long delay = 10;
            gd_GIF *gif;
            SdFat *sd;
            File fd;
            uint8_t *color, *frame;
            CRGB *leds;
            bool playing;

            void cleanup();
    };
}

extern PixelFrame::MediaPlayerClass MediaPlayer;
