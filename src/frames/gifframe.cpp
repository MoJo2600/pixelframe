#pragma once

#include <iostream>
#include <string>
#include <FastLED_NeoMatrix.h>
#include "config.hpp"
#include "frames/gifframe.hpp"

using namespace std;

const std::string EVENT_ID_RANDOM = "frame.event.gif.random";
const std::string EVENT_ID_SINGLE = "frame.event.gif.single";

Frame* RandomGifFrameEvent::getFrame() {
  return new GifFrame();
}

std::string RandomGifFrameEvent::getEventId(void) {
  return EVENT_ID_RANDOM;
}

Frame* SingleGifFrameEvent::getFrame() {
  return new GifFrame();
}

std::string SingleGifFrameEvent::getEventId(void) {
  return EVENT_ID_SINGLE;
}

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
}

void GifFrame::react(FrameEvent* event) {
  if (event->getEventId() == EVENT_ID_RANDOM) {
    cout << "[FRAME::GIF] reacting to random gif event" << endl;
    this->playGif("/gifs/bird.gif");
  } else if (event->getEventId() == EVENT_ID_SINGLE) {
    cout << "[FRAME::GIF] reacting to single gif event" << endl;
    auto e = (SingleGifFrameEvent*) event;
    auto gifPath = "/gifs/" + e->filename;
    cout << "[FRAME::GIF] path to single gif: " << gifPath << endl;
    this->playGif(gifPath);
  } else {
    // TODO:
  }
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

void GifFrame::playGif(std::string filename) {
  cout << "Got play gif event with file: " << filename << endl;
  // image = e.file;
  matrix->clear();
  //TODO: Move to sub states, that will use the gif decoder
  if (file) file.close();
  file = LittleFS.open(filename.c_str(), "r");
  if (!file) {
    cout << "[PIXELFRAME] Error opening GIF file" << endl;
  }
  cout << "[PIXELFRAME] Opened GIF file, start decoding" << endl;
  decoder->startDecoding();
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
  CRGB color = CRGB(matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
  matrix->drawPixel(x, y, color);
}

bool GifFrame::fileSeekCallback(unsigned long position) {
  return file.seek(position);
}

fs::File GifFrame::file;
GifDecoder<16, 16, 10> * GifFrame::decoder;
