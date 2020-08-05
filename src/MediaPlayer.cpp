#include "MediaPlayer.hpp"
#include "SdFat.h"
#include "FastLED.h"

using namespace PixelFrame;

MediaPlayerClass::MediaPlayerClass(){};

MediaPlayerClass::~MediaPlayerClass(){};

void MediaPlayerClass::setup(CRGB *ledConfig, SdFat *sdFat){
  // TODO: add nulptr check
  leds = ledConfig;
  sd = sdFat;
};

void MediaPlayerClass::play(char *filename) {
  fprintf(stdout, "Playing %s\n", filename);
  if (playing)
  {
    stop();
  }

  // TODO: different behaviour for gif / bmp maybe?
  fd = sd->open(filename);
  gif = gd_open_gif(&fd);
  fprintf(stdout, "loaded gif width: %d, height %d\n", gif->width, gif->height);
  frame = (uint8_t *)malloc(gif->width * gif->height * 3);
  playing = true;
}

void MediaPlayerClass::stop() {
  playing = false;
  cleanup(); // Hm... not sure about cleanup
}

void MediaPlayerClass::cleanup() {
  // closes file and frees gif
  gd_close_gif(gif);
  free(frame);
}

void MediaPlayerClass::loop() {
  if (playing) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= delay)
    {
      // fprintf(stdout, "in loop gif width: %d, height %d\n", gif->width, gif->height);
      // fprintf(stdout, "file position in loop: %d\n", gif->fd->position());

      int ret = gd_get_frame(gif);
      if (ret == -1)
      {
        Serial.println("Could not load gif frame");
        return;
      }

      delay = gif->gce.delay+1 * 100;
      gd_render_frame(gif, frame);

      color = frame;
      int startIndex = 0;
      int index = 0;

      for (int i = 0; i < gif->height; i++)
      {
        startIndex = i * 16;
        index = 0;
        for (int j = 0; j < gif->width; j++) {
          if (i % 2 == 0) {
              index = startIndex + j;
          } else {
              index = startIndex + 15 - j;
          }
          leds[index].setRGB(color[0], color[1], color[2]);
          color += 3;
        }
      }
      FastLED.show();

      if (ret == 0) {
        delay = 0;
        gd_rewind(gif);
      }

      previousMillis = millis();
    }
  }
};

MediaPlayerClass MediaPlayer;
