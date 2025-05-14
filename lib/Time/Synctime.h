#include <Arduino.h>
#include <time.h>
#include <Inkplate.h>


#ifndef SYNCTIME_H
#define SYNCTIME_H


class Synctime
{
    public:
        uint8_t hour, minutes, seconds, weekday, day, month;
        uint16_t year;
        static Inkplate *display;
        void setTime();
        void getRtcDate();
        static const long gmtOffset_sec = 3600L * -8;
        static const int daylightOffset_sec = 3600;

    private:
        void printTime(uint8_t hour, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t weekday, uint8_t month,
               uint16_t year);
        void print2Digits(uint8_t d);

};

#endif