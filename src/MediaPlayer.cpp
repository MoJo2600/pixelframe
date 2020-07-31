#include "MediaPlayer.hpp"
#include "SdFat.h"
#include "FastLED.h"

using namespace PixelFrame;

MediaPlayerClass::MediaPlayerClass(){};

MediaPlayerClass::~MediaPlayerClass(){
  free(frame);
};

void MediaPlayerClass::setup(CRGB *ledConfig, SdFat *sdFat){
  leds = ledConfig;
  sd = sdFat;
};

void MediaPlayerClass::play(char *filename) {
  fprintf(stdout, "Playing %s\n", filename);
  if (playing)
  {
    stop();
  }

  // fprintf(stdout, "Opening file\n");
  // File fd = sd->open(filename);
  // if (!fd) {
  //   fprintf(stdout, "open failed");
  //   return;
  // }

  // TODO: different behaviour for gif / bmp maybe?
  fd = sd->open(filename);
  gif = gd_open_gif(&fd);
  fprintf(stdout, "loaded gif width: %d, height %d\n", gif->width, gif->height);
  frame = (uint8_t *)malloc(gif->width * gif->height * 3);
  playing = true;

  // int ret = gd_get_frame(gif);
  // fprintf(stdout, "first frame %d\n", ret);

  // fprintf(stdout, "file position: %d\n", gif->fd->position());

}

void MediaPlayerClass::stop() {
  cleanup(); // Hm... not sure about cleanup
  playing = false;
}

void MediaPlayerClass::cleanup() {
  free(frame);
  // closes file and frees gif
  gd_close_gif(gif);
}


void MediaPlayerClass::loop(){
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
      // Serial.print("Rendering took: ");
      // Serial.print(currentMillis - previousMillis);
      // Serial.println(" ,s");

    }
  }





  // void play_gif(File* fd) {
  //   uint8_t *color, *frame;
  //   int ret = 1;
  //   unsigned long previousMillis = 0;
  //   unsigned long delay = 10;

  //   int startIndex = 0;
  //   int index = 0;
  //   unsigned long currentMillis = millis();
  //   int repeat = 0;
  //   // while (fd.openNext(&dirFile, O_RDONLY)) {
  //   //   if (!fd.isHidden() && !fd.isSubDir()) {

  //   //   fd.close();
  //   // }

  //     gif = gd_open_gif(&fd);
  //     delay = gif->gce.delay+1 * 100;

  //     if (gif == NULL) {
  //       Serial.println("failure reading gif");
  //       return;
  //     }

  //     frame = (uint8_t*) malloc(gif->width * gif->height * 3);
  //     repeat = 0;
  //     while (repeat <= 3) // play only thrice
  //     {
  //       currentMillis = millis();
  //       if (currentMillis - previousMillis >= delay)
  //       {
  //         ret = gd_get_frame(gif);
  //         if (ret == -1)
  //         {
  //           Serial.println("Could not load gif frame");
  //           return;
  //         }

  //         gd_render_frame(gif, frame);

  //         color = frame;

  //         for (int i = 0; i < gif->height; i++)
  //         {
  //           startIndex = i * 16;
  //           index = 0;
  //           for (int j = 0; j < gif->width; j++) {
  //             if (i % 2 == 0) {
  //                 index = startIndex + j;
  //             } else {
  //                 index = startIndex + 15 - j;
  //             }
  //             leds[index].setRGB(color[0], color[1], color[2]);
  //             color += 3;
  //           }
  //         }
  //         FastLED.show();

  //         if (ret == 0) {
  //           repeat++;
  //           Serial.println(repeat);
  //           gd_rewind(gif);
  //         }

  //         previousMillis = currentMillis;
  //       }
  //     }
  //     free(frame);
  //     gd_close_gif(gif);
  //   }
  // }
};

MediaPlayerClass MediaPlayer;
