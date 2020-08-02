/*
  PixelFrame

 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 13 - D7
 ** MISO - pin 12 - D6
 ** CLK - pin 14 - D5
 ** CS - pin 4 - D2

 TODO: LED connection
 LED - pin 5 - D1

 created   July 2020
 by Christian Erhardt
 */
// #define _stackSize (6748/4)
#define FS_NO_GLOBALS // otherwise there is a conflict between fs::File and SD File
#include "SPI.h"
#include "lib/stdinout.h"
// #define FASTLED_ALLOW_INTERRUPTS 0 // https://github.com/FastLED/FastLED/issues/306
// #define FASTLED_ESP8266_DMA

#include "ESP8266WiFi.h"
#include "ArduinoJson.h"
#include "MediaPlayer.h"
#include "SdFat.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
#include "ESP8266TrueRandom.h"

#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include "FastLED.h"
#include "fonts/TomThumb.h"



#define MATRIX_TILE_WIDTH   16 // width of EACH NEOPIXEL MATRIX (not total display)
#define MATRIX_TILE_HEIGHT  16 // height of each matrix
#define MATRIX_TILE_H       1  // number of matrices arranged horizontally
#define MATRIX_TILE_V       1  // number of matrices arranged vertically

// Used by NeoMatrix
#define mw (MATRIX_TILE_WIDTH *  MATRIX_TILE_H)
#define mh (MATRIX_TILE_HEIGHT * MATRIX_TILE_V)
#define NUMMATRIX (mw*mh)

uint8_t matrix_brightness = 32;

CRGB matrixleds[NUMMATRIX];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V, 
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG + 
  NEO_TILE_TOP + NEO_TILE_LEFT +  NEO_TILE_PROGRESSIVE);

const uint16_t colors[] = {
  matrix->Color(255, 0, 0), matrix->Color(0, 255, 0), matrix->Color(0, 0, 255) };


// SD Card setup
SdFat sd;
// SdFile dirFile;
File fd;

#define SD_CS_PIN 4

// LED setup
#define FRAMES_PER_SECOND 60 // TODO: implement
#define DATA_PIN          5
#define BRIGHTNESS        48
// #define CANVAS_WIDTH      16
// #define CANVAS_HEIGHT     16
// #define NUM_LEDS          (CANVAS_WIDTH * CANVAS_HEIGHT)
// CRGB leds[NUM_LEDS];
// CRGB leds_buf[NUM_LEDS];

StaticJsonDocument<512> configuration;

WiFiUDP ntpUDP;
// TODO: implement daylight savings time, etc
NTPClient timeClient(ntpUDP, 3600*2);

// Error messages stored in flash.
#define error(msg) sd.errorHalt(F(msg))

// CLOCK
uint8_t
lastSecond = 255, // a moment ago
paddleIndex = 230,
ballX = 112,
ballY = 208,
ballIndex = 216,
secondCounter = 0,
pong_speed = 100,
pong_paddle_left_y = 0,
pong_paddle_right_y = 0,
pong_paddle_left_start = 0,
pong_paddle_right_start = 0,
pong_paddle_left_target = 0,
pong_paddle_right_target = 0;

bool
pong_scored_hour = false,
pong_scored_minute = false,
pong_celebrate = false,
pongclock = false, // pong clock mode
pong_reset = false,
pong_ball_direction = 0,

wifiConnected = false;

unsigned long
pong_celebration_end = 0,
pong_showtime = 0;

unsigned long
fadeStartTime = 0; // millis to end fade

int
ballAngle = 0,
fadeLength = 0; // length in ms for fade up effect
// offsetX = 0, // for translating images x pixels
// offsetY = 0; // for translating images y pixels

// void clearStripBuffer()
// {
//   for (int i = 0; i < NUM_LEDS; i++)
//   {
//     leds[i] = CRGB(0, 0, 0);
//   }
// }

void printDigits(int digits)
{
  // utility function for clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void debugClockDisplay()
{
  // digital clock display of the time
  Serial.print(timeClient.getHours());
  printDigits(timeClient.getMinutes());
  printDigits(timeClient.getSeconds());
  Serial.println();
}

float degToRad(float deg)
{
  float result;
  result = deg * 3.14159265359 / 180;
  return result;
}


void swapYdirection()
{
  if (ballAngle > 90 && ballAngle < 270)
  {
    if (ballAngle > 180)
    {
      ballAngle = 360 - (ballAngle - 180);
    }
    else
    {
      ballAngle = 90 - (ballAngle - 90);
    }
  }
  else
  {
    if (ballAngle < 90)
    {
      ballAngle = 90 + (90 - ballAngle);
    }
    else
    {
      ballAngle = 180 + (360 - ballAngle);
    }
  }
}

void swapXdirection()
{
  if (ballAngle < 180)
  {
    if (ballAngle < 90)
    {
      ballAngle = 270 + (90 - ballAngle);
    }
    else ballAngle = 270 - (ballAngle - 90);
  }
  else
  {
    if (ballAngle > 270)
    {
      ballAngle = 360 - ballAngle;
    }
    else ballAngle = 180 - (ballAngle - 180);
  }
}

void drawDigits()
{
  matrix->clear();
  // CRGB(rgb2hsv_approximate(CRGB(205,205,205)))
  matrix->setTextColor(matrix->Color(155, 155, 155));
  matrix->setTextSize(1);
  matrix->setFont(&TomThumb);
  matrix->setCursor(0, 10);
  if (timeClient.getHours() < 10)
    matrix->print('0');
  matrix->print(timeClient.getHours());
  matrix->setCursor(9, 10);
  if (timeClient.getMinutes() < 10)
    matrix->print('0');
  matrix->print(timeClient.getMinutes());
  // matrix->show();
  // clearStripBuffer();
  // clockDigit_1();
  // clockDigit_2();
  // clockDigit_3();
  // clockDigit_4();
  // if (!clockSet) clockSetBlink = !clockSetBlink;
  // closeMyFile();
}

long realRandom(int max)
{
  if (0 == max) {
    return 0;
  }
  long random = ESP8266TrueRandom.random(0,max);
  Serial.println(random);
  return random;
}

int realRandom(int min, int max)
{
  if (min >= max) {
    return min;
  }
  return realRandom(max - min) + min;
}

byte getScreenIndex(byte x, byte y)
{
  byte screenX = x / 16;
  byte screenY = y / 16;
  byte index;
  index = screenY * 16;
  if (screenY == 0)
  {
    index = 15 - screenX;
  }
  else if (screenY % 2 != 0)
  {
    index = (screenY * 16) + screenX;
  }
  else
  {
    index = (screenY * 16 + 15) - screenX;
  }
  return index;
}

void fastledshow()
{
  // if (fadeStartTime + fadeLength > millis())
  // {
  //   memmove( leds_buf, leds, NUM_LEDS * sizeof( CRGB) );
  //   uint8_t fadeAmount = map(millis(), fadeStartTime, fadeStartTime + fadeLength, 255, 0);
  //   for (int i = 0; i < NUM_LEDS; i++)
  //   {
  //     leds[i].fadeToBlackBy(fadeAmount);
  //   }
  // }
  // FastLED.show();
  // Serial.println("fastled show");
  matrix->show();
}

int pong_predict_y(int x, int y, int angle)
{
  while (x >= 16 && x <= 256-16)
  {
    if ((y + cos(degToRad(angle)) * 16) + .5 < 0)
    {
      if (angle > 90 && angle < 270)
      {
        if (angle > 180) angle = 360 - (angle - 180);
        else angle = 90 - (angle - 90);
      }
      else
      {
        if (angle < 90) angle = 90 + (90 - angle);
        else angle = 180 + (360 - angle);
      }
    }
    else if ((y + cos(degToRad(angle)) * 16) + .5 > 256)
    {
      if (angle > 90 && angle < 270)
      {
        if (angle > 180) angle = 360 - (angle - 180);
        else angle = 90 - (angle - 90);
      }
      else
      {
        if (angle < 90) angle = 90 + (90 - angle);
        else angle = 180 + (360 - angle);
      }
    }
    x = x + sin(degToRad(angle)) * 16 + .5;
    y = y + cos(degToRad(angle)) * 16 + .5;
  }
  return y;
}

void showClock()
{
  if (timeClient.getSeconds() != lastSecond)
  {
    secondCounter++;
    lastSecond = timeClient.getSeconds();
    debugClockDisplay();

    if (timeClient.getSeconds() == 0)
    {
      // register pong score change
      if (timeClient.getMinutes() == 0) pong_scored_hour = true;
      else pong_scored_minute = true;
    }
  }

  // game in progress
  if (!pong_celebrate)
  {
    if (millis() > pong_showtime)
    {
      pong_showtime = millis() + pong_speed;

      // move ball
      // left side
      if ((ballX + sin(degToRad(ballAngle)) * 16) + .5 > 256-16)
      {
        if (!pong_scored_minute) 
        {
          if (getScreenIndex(ballX, ballY) < getScreenIndex(255, pong_paddle_left_y)) ballAngle = realRandom(225-25, 225+25);
          else if (getScreenIndex(ballX, ballY) == getScreenIndex(255, pong_paddle_left_y) + 1) swapXdirection();
          else if (getScreenIndex(ballX, ballY) == getScreenIndex(255, pong_paddle_left_y) - 1) swapXdirection();
          else ballAngle = realRandom(315-25, 315+25);
          pong_ball_direction = 1;
          pong_paddle_right_start = pong_paddle_right_y;
          pong_paddle_right_target = constrain(pong_predict_y(ballX, ballY, ballAngle), 16, 255-16);
        }
        else
        {
          pong_celebrate = true;
          pong_celebration_end = millis() + 2000;
          // 24 hour conversion
          drawDigits();
          fastledshow();
          // memcpy( leds_buf, leds, NUM_LEDS * sizeof( CRGB) );
        }
      }

      // right side
      else if ((ballX + sin(degToRad(ballAngle)) * 16) + .5 < 16)
      {
        if (!pong_scored_hour)
        {
          if (getScreenIndex(ballX, ballY) < getScreenIndex(0, pong_paddle_right_y)) ballAngle = realRandom(135-25, 135+25);
          else if (getScreenIndex(ballX, ballY) == getScreenIndex(0, pong_paddle_right_y) + 1) swapXdirection();
          else if (getScreenIndex(ballX, ballY) == getScreenIndex(0, pong_paddle_right_y) - 1) swapXdirection();
          else ballAngle = realRandom(45-25, 45+25);
          pong_ball_direction = 0;
          pong_paddle_left_start = pong_paddle_left_y;
          pong_paddle_left_target = constrain(pong_predict_y(ballX, ballY, ballAngle), 16, 255-16);
        }
        else
        {
          pong_celebrate = true;
          pong_celebration_end = millis() + 2000;
          drawDigits();
          fastledshow();
          // memcpy( leds_buf, leds, NUM_LEDS * sizeof( CRGB) );
        }
      }

      if ((ballY + cos(degToRad(ballAngle)) * 16) + .5 < 0) swapYdirection();
      else if ((ballY + cos(degToRad(ballAngle)) * 16) + .5 > 256) swapYdirection();

      ballX = ballX + sin(degToRad(ballAngle)) * 16 + .5;
      ballY = ballY + cos(degToRad(ballAngle)) * 16 + .5;
    }
  }

  // celebrating
  else if (millis() > pong_celebration_end) pong_reset = true;

  // reset?
  if (pong_reset)
  {
    pong_reset = false;
    pong_celebrate = false;
    pong_scored_hour = false;
    pong_scored_minute = false;

    // store second hands
    // Serial.println("Storing second hand colors...");
    // offsetX = 0;
    // offsetY = 176;
    // bmpDraw(clockFace, 0, 0);
    // uint8_t second_index = 0;
    // for (uint8_t x=0; x<16; x++) secondHands[second_index++] = leds[getIndex(x, 0)];
    // for (uint8_t y=0; y<16; y++) secondHands[second_index++] = leds[getIndex(15, y)];
    // for (uint8_t x=0; x<16; x++) secondHands[second_index++] = leds[getIndex(15-x, 15)];
    // for (uint8_t y=0; y<16; y++) secondHands[second_index++] = leds[getIndex(0, 15-y)];

    // 24 hour conversion
    drawDigits();
    fastledshow();
    // memcpy( leds_buf, leds, NUM_LEDS * sizeof( CRGB) );
    pong_paddle_left_y = 128;
    pong_paddle_left_start = 128;
    pong_paddle_left_target = 128;
    pong_paddle_right_y = 128;
    pong_paddle_right_start = 128;
    pong_paddle_right_target = 128;
    ballX = 255-16;
    ballY = 128;
    ballAngle = realRandom(225, 315);
    pong_ball_direction = 1;
    pong_paddle_right_target = constrain(pong_predict_y(ballX, ballY, ballAngle), 16, 255-16);
  }

  // manage paddles
  if (pong_ball_direction == 0) 
  {
    int offset = 0;
    if (pong_scored_minute)
    {
      if (pong_paddle_left_target < 3 * 16) offset = 2 * 16;
      else offset = -2 * 16;
    }
    pong_paddle_left_y = map(ballX, 16, 256-16, pong_paddle_left_start, pong_paddle_left_target + offset);
  }
  else
  {
    int offset = 0;
    if (pong_scored_hour)
    {
      if (pong_paddle_right_target < 3 * 16) offset = 2 * 16;
      else offset = -2 * 16;
    }
    pong_paddle_right_y = map(ballX, 256-16, 16, pong_paddle_right_start, pong_paddle_right_target + offset);
  }

  // copy the time back on screen
  // memcpy( leds, leds_buf, NUM_LEDS * sizeof( CRGB) );

  // get hue
  // secondHands[currentSecond]
  // kein peil was hier passiert
  CHSV pongHue_ball = rgb2hsv_approximate(CRGB(0,0,255));
  //CHSV pongHue_ball = CHSV(160, 255, 255);
  CHSV pongHue_paddle = pongHue_ball;
  pongHue_paddle.hue += 128;

  if (pong_celebrate) pongHue_ball.hue += millis();

  // draw the dots
  drawDigits();
  // matrix->ColorHSV()

  // Serial.print("Paddle Left (");
  // Serial.print("0,");
  // Serial.print(getScreenIndex(255, pong_paddle_left_y)/16);
  // Serial.print(")");
  // Serial.print("Paddle Right (");
  // Serial.print("16,");
  // Serial.print(getScreenIndex(0, pong_paddle_right_y)/16);
  // Serial.println(")");

  matrix->drawPixel(15, getScreenIndex(255, pong_paddle_left_y)/16-1, CRGB(pongHue_paddle));
  matrix->drawPixel(15, getScreenIndex(255, pong_paddle_left_y)/16, CRGB(pongHue_paddle));
  matrix->drawPixel(15, getScreenIndex(255, pong_paddle_left_y)/16+1, CRGB(pongHue_paddle));
  matrix->drawPixel(0, getScreenIndex(0, pong_paddle_right_y)/16-1, CRGB(pongHue_paddle));
  matrix->drawPixel(0, getScreenIndex(0, pong_paddle_right_y)/16, CRGB(pongHue_paddle));
  matrix->drawPixel(0, getScreenIndex(0, pong_paddle_right_y)/16+1, CRGB(pongHue_paddle));


  // matrix->drawFastVHLine(15, getScreenIndex(255, pong_paddle_left_y)/16, 3, CRGB(pongHue_paddle));
  // matrix->drawFastVLine(0, getScreenIndex(0, pong_paddle_right_y)/16, 3, CRGB(pongHue_paddle));
  matrix->drawPixel(ballX/16, ballY/16, CRGB(pongHue_ball));

  // Serial.println("Screen Inddex: ");
  // Serial.println(getScreenIndex(255, pong_paddle_left_y));
  // Serial.println(getScreenIndex(0, pong_paddle_right_y));

  // leds[getScreenIndex(255, pong_paddle_left_y)] = pongHue_paddle;
  // leds[getScreenIndex(255, pong_paddle_left_y-16)] = pongHue_paddle;
  // leds[getScreenIndex(255, pong_paddle_left_y+16)] = pongHue_paddle;
  // leds[getScreenIndex(0, pong_paddle_right_y)] = pongHue_paddle;
  // leds[getScreenIndex(0, pong_paddle_right_y-16)] = pongHue_paddle;
  // leds[getScreenIndex(0, pong_paddle_right_y+16)] = pongHue_paddle;
  // ballIndex = getScreenIndex(ballX, ballY);
  // leds[ballIndex] = pongHue_ball;
  fastledshow();
}

void initClock()
{
  // saveSettingsToEEPROM();

  // alertPhase = 0;
  secondCounter = 0;
  // menuActive = false;
  // clockShown = true;
  // clockAnimationActive = false;
  // buttonTime = millis();
  // menuMode = 0;
  lastSecond = 255;
  pong_reset = true;

  // closeMyFile();
  // holdTime = 0;
  // sd.chdir("/");
  // fileIndex = 0;
  // singleGraphic = true;
  // if (!logoPlayed) logoPlayed = true;

  // if (!clockIniRead) readClockIni();

  // if (!enableSecondHand && framePowered)
  // {
  //   // 24 hour conversion
  //   if (hour12 && currentHour > 12) currentHour -= 12;
  //   if (hour12 && currentHour == 0) currentHour = 12;
    drawDigits();
    fastledshow();
  // }
}

// END CLOCK

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  delay(500);

  // FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is assumed
  // FastLED.setBrightness(BRIGHTNESS);

  FastLED.addLeds<WS2812B,DATA_PIN>(matrixleds, NUMMATRIX); 
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(40);
  // matrix->setTextColor(colors[0]);

  // clearStripBuffer();
  // fastledshow();

  matrix->clear();
  // End Black screen

  // stdout to serial setup
  hal_printf_init();

  Serial.print("Initializing SD card... ");
  if (!sd.begin(SD_CS_PIN)) {
    error("initialization failed!");
    return;
  }
  Serial.println("ok");


  // MediaPlayer.setup(leds, &sd);

  // Switch to bmp
  // MediaPlayer.play("system/nowifi.gif");

  // ### READ CONFIG
  Serial.print("Opening configuration file... ");
  if (!fd.open("system/config.json", O_READ))
  {
    error("opening config failed");
    return;
  }
  Serial.println("ok");

  Serial.println("reading json config");
  char jsonData[fd.size() + 1];
  int stringIndex = 0;
  int data;
  while ((data = fd.read()) >= 0)
  {
    jsonData[stringIndex] = data;
    stringIndex++;
  }
  jsonData[stringIndex] = '\0'; // Add the NULL
  fd.close();

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(configuration, jsonData);
  if (error)
    Serial.println(F("Failed to read file, using default configuration"));

  // ### END: READ CONFIG

  // ### CONNECT WIFI
  const char* ssid = configuration["wifi"]["ssid"];
  const char* password = configuration["wifi"]["password"];

  // Serial.println("Clear screen");

  Serial.println("Connect wifi");

  WiFi.begin(ssid, password);

  // timeClient.begin();

  // MediaPlayer.play("/system/nowifi.gif");

  // if (!dirFile.open("/")) {
  //   error("open root failed");
  // }
  initClock();
}

long lastClockMillis = 0;

void loop() {
  // //
  // MediaPlayer.loop();
  // // delay(2);

  // Wifi Connection
  if (!wifiConnected) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Wifi connected!");
      wifiConnected = true;
      // MediaPlayer.stop();
      // MediaPlayer.play("/system/wifi.gif");
    }
  } else {
    // MediaPlayer.stop();
    timeClient.update();

    if (millis() - lastClockMillis > 20) {
      showClock();
      lastClockMillis = millis();
    }
  }
}
