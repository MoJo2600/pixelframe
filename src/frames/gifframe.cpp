#pragma once

#include <iostream>
#include <FastLED_NeoMatrix.h>
#include "frames/gifframe.hpp"

using namespace std;

void GifFrame::loop(void) {
  if (decoder) {
    decoder->loop();
  }
}

void GifFrame::enter(void) {
  decoder = new GifDecoder<16, 16, 10>();
  decoder->setFilePositionCallback(GifFrame::filePositionCallback);
  decoder->setFileReadCallback(GifFrame::fileReadCallback);
  decoder->setFileReadBlockCallback(GifFrame::fileReadBlockCallback);
  decoder->setScreenClearCallback(GifFrame::screenClearCallback);
  decoder->setUpdateScreenCallback(GifFrame::updateScreenCallback);
  decoder->setDrawPixelCallback(GifFrame::drawPixelCallback);
  decoder->setFileSeekCallback(GifFrame::fileSeekCallback);

  const char * filename = "/gifs/city.gif";
  cout << "Got play gif event with file: " << filename << endl;
  // image = e.file;
  this->pixelMatrix->clear();
  //TODO: Move to sub states, that will use the gif decoder
  if (file) file.close();
  file = LittleFS.open(filename, "r");
  if (!file) {
    cout << "[PIXELFRAME] Error opening GIF file" << endl;
  }
  cout << "[PIXELFRAME] Opened GIF file, start decoding" << endl;
  decoder->startDecoding();
}

void GifFrame::exit(void) {
  cout << "[PIXELFRAME] Exit Gif mode" << endl;
  decoder->stop();
  if (file) {
    cout << "[PIXELFRAME] Close file" << endl;
    file.close();
  } 
  cout << "[PIXELFRAME] Delete decoder" << endl;
  delete decoder;
}

unsigned long GifFrame::filePositionCallback() {
  return file.position();
}

int GifFrame::fileReadCallback() {
  return file.read();
}

int GifFrame::fileReadBlockCallback(void * buffer, int numberOfBytes) {
  return file.read((uint8_t*)buffer, numberOfBytes);
}

void GifFrame::screenClearCallback(void) {
}

void GifFrame::updateScreenCallback(void) {
}

void GifFrame::drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
  CRGB color = CRGB(Frame::pixelMatrix->gamma[red], Frame::pixelMatrix->gamma[green], Frame::pixelMatrix->gamma[blue]);
  Frame::pixelMatrix->drawPixel(x, y, color);
}

bool GifFrame::fileSeekCallback(unsigned long position) {
  return file.seek(position);
}

fs::File GifFrame::file;
GifDecoder<16, 16, 10> * GifFrame::decoder;
