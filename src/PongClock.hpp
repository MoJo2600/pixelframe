#ifndef PONGCLOCK_HPP_INCLUDED
#define PONGCLOCK_HPP_INCLUDED

#include "FastLED_NeoMatrix.h"
#include "ezTime.h"

class PongClockClass {
    public:
        //, NTPClient &client
        PongClockClass(Timezone * tz);
        ~PongClockClass();
        void setup();
        void start();
        void stop();
        void loop();

    private:
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
        pong_ball_direction = 0;

        unsigned long
        pong_celebration_end = 0,
        pong_showtime = 0;

        unsigned long
        fadeStartTime = 0; // millis to end fade

        int
        ballAngle = 0,
        fadeLength = 0,
        current_minute = 0,
        current_hour = 0;

        Timezone * tz;

        void printDigits(int digits);
        void debugClockDisplay();
        float degToRad(float deg);
        void swapYdirection();
        void swapXdirection();
        void drawDigits();
        long realRandom(int max);
        int realRandom(int min, int max);
        byte getScreenIndex(byte x, byte y);
        int pong_predict_y(int x, int y, int angle);
};

#endif // PONGCLOCK_HPP_INCLUDED