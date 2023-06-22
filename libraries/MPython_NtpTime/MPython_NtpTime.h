#ifndef NTPTIME_H
#define NTPTIME_H
#include <stdio.h>
#if defined(ARDUINO_ARCH_ESP8266)
#include "ESP8266WiFi.h"
#else
#include <WiFi.h>
#endif
#include "time.h"

class MPython_NtpTime{
    public:
        struct tm timeinfo;
        
        typedef enum{
            UTCWest12_t = -12,
            UTCWest11_t = -11,
            UTCWest10_t = -10,
            UTCWest9_t = -9,
            UTCWest8_t = -8,
            UTCWest7_t = -7,
            UTCWest6_t = -6,
            UTCWest5_t = -5,
            UTCWest4_t = -4,
            UTCWest3_t = -3,
            UTCWest2_t = -2,
            UTCWest1_t = -1,
            UTCMiddle_t = 0,
            UTCEast1_t = 1,
            UTCEast2_t = 2,
            UTCEast3_t = 3,
            UTCEast4_t = 4,
            UTCEast5_t = 5,
            UTCEast6_t = 6,
            UTCEast7_t = 7,
            UTCEast8_t = 8,
            UTCEast9_t = 9,
            UTCEast10_t = 10,
            UTCEast11_t = 11,
            UTCEast12_t = 12,
        }timeZone_t;

        typedef enum{
            Year,
            Month,
            Date,
            Hour,
            Minute,
            Second,
            Week,
        }timeType_t;

        uint32_t localTime(timeType_t type);
        void setNtpTime(timeZone_t timeZone, String ntpServer);
    private:
        //const char* daysOfTheWeek[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        //const char* month[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
		const uint8_t daysOfTheWeek[7] = {1, 2, 3, 4, 5, 6, 7};
        const uint8_t month[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
};
#endif