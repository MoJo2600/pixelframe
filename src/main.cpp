/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
//#include <SD.h>
#include "SdFat.h"
#include "lib/gifdec.h"
#include "lib/stdinout.h"

SdFat sd;
File fd;

#define SD_CS_PIN 4

unsigned long previousMillis = 0;
gd_GIF *gif;

void setup() {

  char sigver[3];
  int n;
  uint16_t width, height, depth;

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // stdout to serial setup
  hal_printf_init();

  Serial.print("Initializing SD card... ");

  if (!sd.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  fd = sd.open("mario.gif", O_RDONLY);
  if (!fd) {
    sd.errorHalt("open failed");
  }

  // // // char fileName[] = "mario.gif";
  // int fd = ifstream.open("mario.gif", O_RDONLY);
  // if (!fd) {
  //   Serial.println("could not open file");
  //   return;
  // }

  Serial.println("Reading gif... ");
  Serial.println("---------------");
  gif = gd_open_gif(fd);
  Serial.println("---------------");

  if (gif == NULL) {
    Serial.println("failure reading gif");
    return;
  }
  Serial.println("ok");

  uint8_t *color, *frame;
  int ret;

  frame = (uint8_t*) malloc(gif->width * gif->height * 3);
  unsigned long delay = 0;

  ret = gd_get_frame(gif);
  Serial.println("get_frame");
  Serial.println(ret);
  if (ret == -1)
  {
    Serial.println("Could not load gif frame");
    return;
  }

  // color = &gif->gct.colors[gif->bgindex * 3];

  gd_render_frame(gif, frame);
  color = frame;
  for (int i = 0; i < gif->height; i++) {
      for (int j = 0; j < gif->width; j++) {
          if (color[0] == 0 && color[1] == 0 && color[2] == 0) {
            Serial.print(" ");
          } else {
            Serial.print("#");
          }
          color += 3;
      }
      Serial.print("\n");
  }
  // Serial.println("width");
  // Serial.println(gif->width);
  // Serial.println("height");
  // Serial.println(gif->height);

  // open test file
  // SdFile rdfile("mario.gif", O_RDONLY);  

  // check for open error
  // if (!rdfile.isOpen()) {
  //   error("demoFgets");
  // }

  // /* Header */
  // // read(fd, sigver, 3);
  // n = rdfile.fgets(sigver, 4);
  // Serial.println("Signature ");
  // Serial.println(n);
  // Serial.println(sigver);
  // if (memcmp(sigver, "GIF", 3) != 0)
  // {
  //   Serial.println("Invalid signature.");
  //     // fprintf(stderr, "invalid signature\n");
  //     // goto fail;
  // }

  // /* Version */
  // n = rdfile.fgets(sigver, 4);
  // Serial.println("Version ");
  // Serial.println(n);
  // Serial.println(sigver);
  // if (memcmp(sigver, "89a", 3) != 0) {
  //     Serial.println("Invalid version.");
  // }

  // /* Width x Height */
  // width  = read_num(rdfile);
  // height = read_num(rdfile);

  // Serial.println("width");
  // Serial.println(width);
  // Serial.println("height");
  // Serial.println(height);

  // uint8_t fdsz;
  // char fdsz_char[1];
  
  // /* FDSZ */
  // rdfile.fgets(fdsz_char, 2);
  // fdsz = (uint8_t)fdsz_char;
  // // read(fd, &fdsz, 1);
  // Serial.println('FDSZ');
  // Serial.println(fdsz);
  // /* Presence of GCT */
  // if (!(fdsz & 0x80)) {
  //     Serial.println("no global color table.");
  //     // fprintf(stderr, "no global color table\n");
  //     // goto fail;
  // }

  // // open the file. note that only one file can be open at a time,
  // // so you have to close this one before opening another.
  
  // myFile = SD.open("test.txt", FILE_WRITE);

  // // if the file opened okay, write to it:
  // if (myFile) {
  //   Serial.print("Writing to test.txt...");
  //   myFile.println("testing 1, 2, 3.");
  //   // close the file:
  //   myFile.close();
  //   Serial.println("done.");
  // } else {
  //   // if the file didn't open, print an error:
  //   Serial.println("error opening test.txt");
  // }

  // // re-open the file for reading:
  // myFile = SD.open("test.txt");
  // if (myFile) {
  //   Serial.println("test.txt:");

  //   // read from the file until there's nothing else in it:
  //   while (myFile.available()) {
  //     Serial.write(myFile.read());
  //   }
  //   // close the file:
  //   myFile.close();
  // } else {
  //   // if the file didn't open, print an error:
  //   Serial.println("error opening test.txt");
  // }
}

void loop() {
  // // nothing happens after setup
  // unsigned long currentMillis = millis();
  // if (currentMillis - previousMillis >= delay)
  // {
  //     previousMillis = currentMillis;

  //     ret = gd_get_frame(gif);
  //     if (ret == -1)
  //         return ret;

  //     gd_render_frame(gif, frame);
  //     color = frame;
  //     for (int i = 0; i < gif->height; i++) {
  //         for (int j = 0; j < gif->width; j++) {
  //             //addr = surface->pixels + (i * surface->pitch + j * sizeof(pixel));
  //             //pixel = SDL_MapRGB(surface->format, color[0], color[1], color[2]);
  //             //memcpy(addr, &pixel, sizeof(pixel));
  //             if (color[0] == 0 && color[1] == 0 && color[2] == 0) {
  //                 printf(" ");
  //             } else {
  //                 int ansicolor = rgbToAnsi256(color[0], color[1], color[2]);
  //                 printf("\u001b[38;5;%dm0", ansicolor);
  //             }
              
  //             // printf("Pixel %u, %u, %u", color[0], color[1], color[2]);
  //             color += 3;
  //         }
  //         printf("\n");
  //     }

  //     delay = gif->gce.delay * 10;
  //     // printf("delay: %d", delay);

  //     if (ret == 0)
  //         gd_rewind(gif);
  // }
  // // loop();
}

