#include "PongClock.hpp"
#include "ezTime.h"
#include "fonts/TomThumbPatched.h"
#include "config.hpp"

// #define DEBUG
/************************************************
  PongClock

  based on the awesome work of Jeremy Williams
  https://github.com/Jerware/GameFrameV2
  LEDSEQ.COM
*************************************************/
PongClockClass::PongClockClass(Timezone * tz) : tz{tz} {};

PongClockClass::~PongClockClass(){ };

void PongClockClass::start() { }

void PongClockClass::stop() { }

void PongClockClass::printDigits(int digits)
{
  // utility function for clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void PongClockClass::debugClockDisplay()
{
  // digital clock display of the time
  Serial.print(tz->hour());
  printDigits(tz->minute());
  printDigits(tz->second());
  Serial.println();
}

float PongClockClass::degToRad(float deg)
{
  float result;
  result = deg * 3.14159265359 / 180;
  return result;
}

void PongClockClass::swapYdirection()
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

void PongClockClass::swapXdirection()
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

void PongClockClass::drawDigits()
{
  // matrix->clear();
  matrix->setTextColor(matrix->Color(155, 155, 155));
  matrix->setTextSize(1);
  matrix->setFont(&TomThumbPatched);
  matrix->setCursor(0, 10);
  if (current_hour < 10)
    matrix->print('0');
  matrix->print(current_hour);
  matrix->setCursor(9, 10);
  if (current_minute < 10)
    matrix->print('0');
  matrix->print(current_minute);
}

long PongClockClass::realRandom(int max)
{
  if (0 == max) {
    return 0;
  }
  long rand = random(0,max);
  return rand;
}

int PongClockClass::realRandom(int min, int max)
{
  if (min >= max) {
    return min;
  }
  return realRandom(max - min) + min;
}

byte PongClockClass::getScreenIndex(byte x, byte y)
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

int PongClockClass::pong_predict_y(int x, int y, int angle)
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

void PongClockClass::loop()
{
  matrix->clear();
  if (tz->second() != lastSecond)
  {
    secondCounter++;
    lastSecond = tz->second();
    
#ifdef DEBUG
    debugClockDisplay();
#endif

    if (tz->second() == 0)
    {
      // register pong score change
      if (tz->minute() == 0) pong_scored_hour = true;
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
          current_hour = tz->hour();
          current_minute = tz->minute();
          drawDigits();
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
          current_hour = tz->hour();
          current_minute = tz->minute();
          drawDigits();
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

    // 24 hour conversion
    drawDigits();
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

#ifdef DEBUG
  Serial.print("Paddle Left (");
  Serial.print("0,");
  Serial.print(getScreenIndex(255, pong_paddle_left_y)/16);
  Serial.print(")");
  Serial.print("Paddle Right (");
  Serial.print("16,");
  Serial.print(getScreenIndex(0, pong_paddle_right_y)/16);
  Serial.println(")");
#endif
  matrix->drawPixel(15, getScreenIndex(255, pong_paddle_left_y)/16-1, CRGB(pongHue_paddle));
  matrix->drawPixel(15, getScreenIndex(255, pong_paddle_left_y)/16, CRGB(pongHue_paddle));
  matrix->drawPixel(15, getScreenIndex(255, pong_paddle_left_y)/16+1, CRGB(pongHue_paddle));
  matrix->drawPixel(0, getScreenIndex(0, pong_paddle_right_y)/16-1, CRGB(pongHue_paddle));
  matrix->drawPixel(0, getScreenIndex(0, pong_paddle_right_y)/16, CRGB(pongHue_paddle));
  matrix->drawPixel(0, getScreenIndex(0, pong_paddle_right_y)/16+1, CRGB(pongHue_paddle));
  matrix->drawPixel(ballX/16, ballY/16, CRGB(pongHue_ball));

  // // matrix->drawFastVHLine(15, getScreenIndex(255, pong_paddle_left_y)/16, 3, CRGB(pongHue_paddle));
  // // matrix->drawFastVLine(0, getScreenIndex(0, pong_paddle_right_y)/16, 3, CRGB(pongHue_paddle));

  // // Serial.println("Screen Inddex: ");
  // // Serial.println(getScreenIndex(255, pong_paddle_left_y));
  // // Serial.println(getScreenIndex(0, pong_paddle_right_y));

  // // leds[getScreenIndex(255, pong_paddle_left_y)] = pongHue_paddle;
  // // leds[getScreenIndex(255, pong_paddle_left_y-16)] = pongHue_paddle;
  // // leds[getScreenIndex(255, pong_paddle_left_y+16)] = pongHue_paddle;
  // // leds[getScreenIndex(0, pong_paddle_right_y)] = pongHue_paddle;
  // // leds[getScreenIndex(0, pong_paddle_right_y-16)] = pongHue_paddle;
  // // leds[getScreenIndex(0, pong_paddle_right_y+16)] = pongHue_paddle;
  // // ballIndex = getScreenIndex(ballX, ballY);
  // // leds[ballIndex] = pongHue_ball;
  // // fastledshow();
}

void PongClockClass::setup(){
  secondCounter = 0;
  lastSecond = 255;
  pong_reset = true;
  matrix->setTextWrap(false);
  current_minute = tz->minute();
  current_hour = tz->hour();
  drawDigits();
}
