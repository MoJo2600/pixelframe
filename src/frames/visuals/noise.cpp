/*
A FastLED matrix example:
A simplex noise field fully modulated and controlled by itself
written by
Stefan Petrick 2017
Do with it whatever you like and show your results to the FastLED community
https://plus.google.com/communities/109127054924227823508
*/
#include <FastLED_NeoMatrix.h>
// #include "config.hpp"
#include "frames/visuals/noise.hpp"
#include <iostream>

DEFINE_GRADIENT_PALETTE(pit) {
  0,     3,   3,   3,
  64,   13,   13, 255,  //blue
  128,   3,   3,   3,
  192, 255, 130,   3 ,  //orange
  255,   3,   3,   3
};

using std::cout; using std::endl;

// matrix size
uint8_t CentreX =  (MATRIX_TILE_WIDTH / 2) - 1;
uint8_t CentreY = (MATRIX_TILE_HEIGHT / 2) - 1;

#define NUM_LAYERS 1
uint32_t x[NUM_LAYERS];
uint32_t y[NUM_LAYERS];
uint32_t z[NUM_LAYERS];
uint32_t scale_x[NUM_LAYERS];
uint32_t scale_y[NUM_LAYERS];
uint8_t noise[1][16][16];

void Noise::enter() {}

void Noise::adjust_gamma()
{
  for (uint16_t i = 0; i < 256; i++)
  {
    matrixleds[i].r = dim8_video(matrixleds[i].r);
    matrixleds[i].g = dim8_video(matrixleds[i].g);
    matrixleds[i].b = dim8_video(matrixleds[i].b);
  }
}

void Noise::loop()
{
  this->make_some_noise();
}

void Noise::make_some_noise()
{
  CRGBPalette16 Pal( pit );

  //modulate the position so that it increases/decreases x
  //(here based on the top left pixel - it could be any position else)
  //the factor "2" defines the max speed of the x movement
  //the "-255" defines the median moving direction
  x[0] = x[0] + (2 * noise[0][0][0]) - 255;
  //modulate the position so that it increases/decreases y
  //(here based on the top right pixel - it could be any position else)
  y[0] = y[0] + (2 * noise[0][MATRIX_TILE_WIDTH-1][0]) - 255;
  //z just in one direction but with the additional "1" to make sure to never get stuck
  //in case the movement is stopped by a crazy parameter (noise data) combination
  //(here based on the down left pixel - it could be any position else)
  z[0] += 1 + ((noise[0][0][MATRIX_TILE_HEIGHT-1]) / 4);
  //set the scaling based on left and right pixel of the middle line
  //here you can set the range of the zoom in both dimensions
  scale_x[0] = 8000 + (noise[0][0][CentreY] * 16);
  scale_y[0] = 8000 + (noise[0][MATRIX_TILE_WIDTH-1][CentreY] * 16);

  //calculate the noise data
  uint8_t layer = 0;
  for (uint8_t i = 0; i < MATRIX_TILE_WIDTH; i++) {
    uint32_t ioffset = scale_x[layer] * (i - CentreX);
    for (uint8_t j = 0; j < MATRIX_TILE_HEIGHT; j++) {
      uint32_t joffset = scale_y[layer] * (j - CentreY);
      uint16_t data = inoise16(x[layer] + ioffset, y[layer] + joffset, z[layer]);
      // limit the 16 bit results to the interesting range
      if (data < 11000) data = 11000;
      if (data > 51000) data = 51000;
      // normalize
      data = data - 11000;
      // scale down that the result fits into a byte
      data = data / 161;
      // store the result in the array
      noise[layer][i][j] = data;
    }
  }

  //map the colors
  for (uint8_t y = 0; y < MATRIX_TILE_HEIGHT; y++) {
    for (uint8_t x = 0; x < MATRIX_TILE_WIDTH; x++) {
      //I will add this overlay CRGB later for more colors
      //it´s basically a rainbow mapping with an inverted brightness mask
      CRGB overlay = CHSV(noise[0][y][x], 255, noise[0][x][y]);
      //here the actual colormapping happens - note the additional colorshift caused by the down right pixel noise[0][15][15]
      matrixleds[XY(x, y)] = ColorFromPalette( Pal, noise[0][MATRIX_TILE_WIDTH-1][MATRIX_TILE_HEIGHT-1] + noise[0][x][y]) + overlay;
    }
  }

  //make it looking nice
  adjust_gamma();
}

RegisterVisual<Noise> Noise::reg("noise");


