#include <iostream>
#include <string>
#include <FastLED_NeoMatrix.h>
#include "filesystem.hpp"
#include "config.hpp"
#include "frames/gifframe.hpp"
#include "esp_heap_caps.h"

using namespace std;

#define GIF_DURATION 30 //TODO: make configurable
#define MAX_GIF_SIZE (256 * 1024)  // 256KB max GIF size

// Static buffer members
uint8_t* GifFrame::gifBuffer = nullptr;
size_t GifFrame::gifBufferSize = 0;
size_t GifFrame::gifBufferPos = 0;
GifDecoder<16, 16, 10>* GifFrame::decoder = nullptr;

const std::string EVENT_ID_RANDOM = "frame.event.gif.random";
const std::string EVENT_ID_SINGLE = "frame.event.gif.single";

Frame *RandomGifFrameEvent::getFrame()
{
  return new GifFrame();
}

std::string RandomGifFrameEvent::getEventId(void)
{
  return EVENT_ID_RANDOM;
}

SingleGifFrameEvent::SingleGifFrameEvent(int dur = 0)
{
  duration = dur;
};

Frame *SingleGifFrameEvent::getFrame()
{
  return new GifFrame();
}

std::string SingleGifFrameEvent::getEventId(void)
{
  return EVENT_ID_SINGLE;
}

void GifFrame::loop(void)
{
  if (this->currentEvent->getEventId() == EVENT_ID_RANDOM)
  {
    EVERY_N_SECONDS(GIF_DURATION) {
      cout << "[FRAME::GIF] switching to next random gif" << endl;
      int gif = rand() % gifs_vec.size();
      String gifPath = gifs_vec[gif];
      this->playGif(std::string(gifPath.c_str()));
    }
  }
  if (decoder)
  {
    decoder->loop();
  }
}

void GifFrame::enter(void)
{
  decoder = new GifDecoder<16, 16, 10>();
  decoder->setFilePositionCallback(GifFrame::filePositionCallback);
  decoder->setFileReadCallback(GifFrame::fileReadCallback);
  decoder->setFileReadBlockCallback(GifFrame::fileReadBlockCallback);
  decoder->setScreenClearCallback(GifFrame::screenClearCallback);
  decoder->setUpdateScreenCallback(GifFrame::updateScreenCallback);
  decoder->setDrawPixelCallback(GifFrame::drawPixelCallback);
  decoder->setFileSeekCallback(GifFrame::fileSeekCallback);
}

void GifFrame::react(FrameEvent *event)
{
  this->currentEvent = event;
  if (event->getEventId() == EVENT_ID_RANDOM)
  {
    cout << "[FRAME::GIF] reacting to random gif event" << endl;

    int gif = rand() % gifs_vec.size();
    String gifPath = gifs_vec[gif];

    this->playGif(std::string(gifPath.c_str()));
  }
  else if (event->getEventId() == EVENT_ID_SINGLE)
  {
    cout << "[FRAME::GIF] reacting to single gif event" << endl;

    auto e = (SingleGifFrameEvent *)event;
    auto gifPath = e->filename;
    cout << "[FRAME::GIF] path to single gif: " << gifPath << endl;

    this->playGif(gifPath);
  }
  else
  {
    // TODO:
  }
}

void GifFrame::exit(void)
{
  cout << "[FRAME::GIF] Exit Gif mode" << endl;
  if (decoder) {
    decoder->stop();
    cout << "[FRAME::GIF] Delete decoder" << endl;
    delete decoder;
    decoder = nullptr;
  }
  freeBuffer();
}

void GifFrame::freeBuffer(void)
{
  if (gifBuffer) {
    cout << "[FRAME::GIF] Freeing GIF buffer (" << gifBufferSize << " bytes)" << endl;
    free(gifBuffer);
    gifBuffer = nullptr;
    gifBufferSize = 0;
    gifBufferPos = 0;
  }
}

bool GifFrame::loadGifToBuffer(std::string filename)
{
  // Free existing buffer
  freeBuffer();
  
  // Open file
  fs::File file = LITTLEFS.open(filename.c_str(), "r");
  if (!file) {
    cout << "[FRAME::GIF] Error opening GIF file: " << filename << endl;
    return false;
  }
  
  size_t fileSize = file.size();
  cout << "[FRAME::GIF] Loading GIF to RAM: " << filename << " (" << fileSize << " bytes)" << endl;
  
  // Check size limit
  if (fileSize > MAX_GIF_SIZE) {
    cout << "[FRAME::GIF] GIF too large! Max: " << MAX_GIF_SIZE << " bytes" << endl;
    file.close();
    return false;
  }
  
  // Try to allocate from PSRAM first (ESP32 has 4MB PSRAM on Lolin D32 Pro)
  gifBuffer = (uint8_t*)heap_caps_malloc(fileSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  
  // Fallback to regular RAM if PSRAM not available
  if (!gifBuffer) {
    cout << "[FRAME::GIF] PSRAM unavailable, trying regular RAM" << endl;
    gifBuffer = (uint8_t*)malloc(fileSize);
  }
  
  if (!gifBuffer) {
    cout << "[FRAME::GIF] Failed to allocate buffer!" << endl;
    file.close();
    return false;
  }
  
  // Read entire file into buffer
  size_t bytesRead = file.read(gifBuffer, fileSize);
  file.close();
  
  if (bytesRead != fileSize) {
    cout << "[FRAME::GIF] Read error: expected " << fileSize << ", got " << bytesRead << endl;
    freeBuffer();
    return false;
  }
  
  gifBufferSize = fileSize;
  gifBufferPos = 0;
  
  cout << "[FRAME::GIF] GIF loaded to RAM successfully" << endl;
  return true;
}

void GifFrame::playGif(std::string filename)
{
  cout << "[FRAME::GIF] Playing: " << filename << endl;
  matrix->clear();
  
  if (!loadGifToBuffer(filename)) {
    cout << "[FRAME::GIF] Failed to load GIF to buffer" << endl;
    return;
  }
  
  cout << "[FRAME::GIF] Starting decoder" << endl;
  decoder->startDecoding();
}

unsigned long GifFrame::filePositionCallback()
{
  return gifBufferPos;
}

int GifFrame::fileReadCallback()
{
  if (gifBuffer && gifBufferPos < gifBufferSize) {
    return gifBuffer[gifBufferPos++];
  }
  return -1;  // EOF
}

int GifFrame::fileReadBlockCallback(void *buffer, int numberOfBytes)
{
  if (!gifBuffer || gifBufferPos >= gifBufferSize) {
    return 0;
  }
  
  // Calculate how many bytes we can actually read
  size_t bytesAvailable = gifBufferSize - gifBufferPos;
  size_t bytesToRead = (numberOfBytes < bytesAvailable) ? numberOfBytes : bytesAvailable;
  
  // Copy from buffer (this is fast - just a memcpy!)
  memcpy(buffer, gifBuffer + gifBufferPos, bytesToRead);
  gifBufferPos += bytesToRead;
  
  return bytesToRead;
}

void GifFrame::screenClearCallback(void)
{
}

void GifFrame::updateScreenCallback(void)
{
}

void GifFrame::drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue)
{
  CRGB color = CRGB(matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
  matrix->drawPixel(x, y, color);
}

bool GifFrame::fileSeekCallback(unsigned long position)
{
  if (position <= gifBufferSize) {
    gifBufferPos = position;
    return true;
  }
  return false;
}
// Static members initialized at top of file
