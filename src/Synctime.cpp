#include "Arduino.h"
#include "Synctime.h"

Inkplate* Synctime::display = nullptr;


void Synctime::setTime() 
{
    uint8_t hour = 0, minutes = 0, seconds = 0, weekday = 0, day = 0, month = 0 ;
    uint16_t year = 0;
    Serial.print("Syncing time with NTP server ");
    struct tm timeinfo;
    time_t now;
    while (!getLocalTime(&timeinfo))
    {
        delay(500);
        Serial.print(".");
        configTime(gmtOffset_sec, daylightOffset_sec, "192.168.23.1");
    }
    hour = timeinfo.tm_hour;
    minutes = timeinfo.tm_min;
    seconds = timeinfo.tm_sec;
    display->rtcSetTime(hour, minutes, seconds);
    year = timeinfo.tm_year + 1900;
    month = timeinfo.tm_mon + 1;
    day = timeinfo.tm_mday;
    weekday = timeinfo.tm_wday;
    display->rtcSetDate(weekday, day, month, year);
    Serial.println(" Done!");
}

void Synctime::getRtcDate()
{
    display->rtcClearAlarmFlag();    // set HIGH GPIO 39
    display->rtcGetRtcData();           
    seconds = display->rtcGetSecond();  
    minutes = display->rtcGetMinute();  
    hour = display->rtcGetHour();       
    day = display->rtcGetDay();         
    weekday = display->rtcGetWeekday(); 
    month = display->rtcGetMonth();     
    year = display->rtcGetYear();       
    printTime(hour, minutes, seconds, day, weekday, month, year);
    display->partialUpdate();
    int next_update = 60 - seconds;
    display->rtcSetAlarmEpoch(display->rtcGetEpoch() + next_update, RTC_ALARM_MATCH_SS);
}

void Synctime::printTime(uint8_t hour, uint8_t minutes, uint8_t seconds, uint8_t day, uint8_t weekday, uint8_t month,
               uint16_t year)
{
    // Write time and date info on screen
    const char *wday[] = {"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};
    display->fillRect(640, 715, 360, 40, WHITE); //clear time area
    display->setTextSize(1);
    display->setCursor(640, 740);
    print2Digits(hour);
    display->print(':');
    print2Digits(minutes);
    // display->print(':');
    // print2Digits(seconds);
    display->print(' ');
    display->print(wday[weekday]);
    display->print(". ");
    print2Digits(day);
    display->print('/');
    print2Digits(month);
    display->print('/');
    display->print(year, DEC);
}

void Synctime::print2Digits(uint8_t d)
{
    if (d < 10)
        display->print('0');
    display->print(d, DEC);
}