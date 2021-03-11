#include "config.hpp"

bool init_done = 0;

uint8_t matrix_brightness = 64;
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint16_t speed = 255;
float matrix_gamma = 1; // higher number is darker, needed for Neomatrix more than SmartMatrix

CRGB* matrixleds = nullptr;
FastLED_NeoMatrix* matrix = new FastLED_NeoMatrix(
  matrixleds, 
  MATRIX_TILE_WIDTH, 
  MATRIX_TILE_HEIGHT, 
  MATRIX_TILE_H, 
  MATRIX_TILE_V,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
      NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG +
      NEO_TILE_TOP + NEO_TILE_LEFT +  NEO_TILE_PROGRESSIVE
);

Timezone* timezone = new Timezone();

int XY2( int x, int y, bool wrap) {
    wrap = wrap; // squelch compiler warning
    return matrix->XY(x,MATRIX_HEIGHT-1-y);
}

uint16_t XY( uint8_t x, uint8_t y) {
    return matrix->XY(x,y);
}

int wrapX(int x) {
    if (x < 0 ) return 0;
    if (x >= MATRIX_WIDTH) return (MATRIX_WIDTH-1);
    return x;
}

void show_free_mem(const char *pre) {
    Framebuffer_GFX::show_free_mem(pre);
}

void die(const char *mesg) {
    Serial.println(mesg);
    while(1) delay((uint32_t)1); // while 1 loop only triggers watchdog on ESP chips
}

void *mallocordie(const char *varname, uint32_t req, bool psram) {
#ifndef BOARD_HAS_PSRAM
    psram = false;
#endif
    if (psram) Serial.print("PS");
    Serial.print("Malloc ");
    Serial.print(varname);
    Serial.print(" . Requested bytes: ");
    Serial.println(req);
    void *mem;
    mem = malloc(req);

    if (mem) {
        return mem;
    } else {
        Serial.print("FATAL: ");
        if (psram) Serial.print("ps_");
        Serial.print("malloc failed for ");
        Serial.println(varname);
        show_free_mem();
        while (1); // delay(1);  Adding this seems to cause an ESP32 bug
    }
    return NULL;
}

void matrix_setup(bool initserial, int reservemem) {
    reservemem = reservemem; // squelch compiler warning if var is unused.
    // It's bad to call Serial.begin twice, so it's disabled here now, make sure you have it enabled
    // in your calling script.
    Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Serial.begin");

    if (init_done) {
        Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> BUG: matrix_setup called twice");
        return;
    }
    init_done = 1;
    show_free_mem("Memory after setup() starts");

    
    // Smartmatrix defines the framebuffers itself. Other methods make their own allocation here  
    // ESP32 has more memory available for allocation in setup than in global
    // (if this were a global array), so we use malloc here.
    // https://forum.arduino.cc/index.php?topic=647833
    matrixleds = (CRGB *) mallocordie("matrixleds", sizeof(CRGB) * NUMMATRIX, 1);
    // and then fix the until now NULL pointer in the object.
    matrix->newLedsPtr(matrixleds);
    show_free_mem("After matrixleds malloc");

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(matrixleds, NUMMATRIX); 

    //============================================================================================
    // Matrix Init End
    //============================================================================================

    show_free_mem("Before matrix->begin");
    matrix->begin();
    //show_free_mem("After matrix->begin");

    Serial.print("Setting Brightness: ");
    Serial.println(matrix_brightness);
    FastLED.setBrightness(matrix_brightness);
    Serial.print("Gamma Correction: ");
    Serial.println(matrix_gamma);
    // Gamma is used by AnimatedGIFs and others, as such:
    // CRGB color = CRGB(matrix->gamma[red], matrix->gamma[green], matrix->gamma[blue]);
    matrix->precal_gamma(matrix_gamma);

    Serial.println("matrix_setup done");

    // ESP32 speed tests
    // - Adafruit::ILI9341 speed at 80Mhz is 
    // - WROVER at 24Mhz is 25fps, doesn't seem to work any faster
    // - https://github.com/loboris/ESP32_TFT_lib (DMA) at 24Mhz is only 7fps?
    //
    // ILI9314: 80Mhz: TFT 40fps, NO PSRAM: 32fps, PSRAM show: 12ps  => unstable, no display
    // ILI9314: 40Mhz: TFT 25fps, NO PSRAM: 21fps, PSRAM show: 10fps => unstable, no display
    // ILI9314: 39Mhz: TFT 18fps, NO PSRAM: 16fps, PSRAM show:  9fps => unstable, garbled
    // ILI9314: 30Mhz: TFT 18fps, NO PSRAM: 16fps, PSRAM show:  9fps => still a bit unstabled, garbled
    // ILI9314: 24Mhz: TFT 14fps, NO PSRAM: 12fps, PSRAM show:  8fps => stable
    // ILI9314: 20Mhz: TFT 14fps, NO PSRAM: 12fps, PSRAM show:  8fps
    //
    // ST7735_128b160: 80Mhz: TFT153fps, NO PSRAM:104fps, PSRAM show: 45fps => unstable, no display
    // ST7735_128b160: 60Mhz: TFT 93fps, NO PSRAM: 73fps, PSRAM show: 38fps
    // ST7735_128b160: 60Mhz: TFT 96fps, NO PSRAM: 52fps
    // ST7735_128b160: 40Mhz: TFT 68fps, NO PSRAM: 56fps, PSRAM show: 32fps
    // ST7735_128b160: 20Mhz: TFT 53fps, NO PSRAM: 45fps, PSRAM show: 29fps
    //
    // ST7735_128b128: 60Mhz: TFT117fps, NO PSRAM: 90fps, PSRAM show: 48fps => unstable, garbled
    // ST7735_128b128: 40Mhz: TFT117fps, NO PSRAM: 90fps, PSRAM show: 48fps => unstable, garbled
    // ST7735_128b128: 32Mhz: TFT 84fps, NO PSRAM: 70fps, PSRAM show: 41fps => stable
    // ST7735_128b128: 20Mhz: TFT 66fps, NO PSRAM: 56fps, PSRAM show: 36fps
    //
    // SSD1331: SWSPI: TFT  9fps, NO PSRAM:  9fps, PSRAM show:  8fps => stable
    uint32_t before;
    before = millis();
    for (uint8_t i=0; i<5; i++) {
      matrix->show();
    }
    Serial.print("Matrix->show() Speed Test fps: ");
    Serial.println((5* 1000/(millis() - before)));

    // At least on teensy, due to some framework bug it seems, early
    // serial output gets looped back into serial input
    // Hence, flush input.
    while(Serial.available() > 0) { char t = Serial.read(); t = t; }
}