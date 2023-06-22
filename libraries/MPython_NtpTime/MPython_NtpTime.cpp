#include "MPython_NtpTime.h"

#if defined(ARDUINO_ARCH_ESP8266)
bool getLocalTime(struct tm * info, uint32_t ms) {
  uint32_t count = ms / 10;
  time_t now;
  time(&now);
  localtime_r(&now, info);
  if (info->tm_year > (2016 - 1900)) {
    return true;
  }

  while (count--) {
    delay(10);
    time(&now);
    localtime_r(&now, info);
    if (info->tm_year > (2016 - 1900)) {
      return true;
    }
  }
  return false;
}
#endif

uint32_t MPython_NtpTime::localTime(timeType_t type)
{
    char localTime[100];
#if defined(ARDUINO_ARCH_ESP8266)
    if(!getLocalTime(&timeinfo, 5000)){
#else
    if(!getLocalTime(&timeinfo)){
#endif
        return -1;
    }
    sprintf(localTime, "%d, %d %d %d %d:%d:%d", daysOfTheWeek[timeinfo.tm_wday], month[timeinfo.tm_mon],
    timeinfo.tm_mday,timeinfo.tm_year+1900,timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
    uint32_t back = 0;
    switch (type) {
        case Year:
            back = timeinfo.tm_year+1900;
            break;
        case Month:
            back = month[timeinfo.tm_mon];
            break;
        case Date:
            back = timeinfo.tm_mday;
            break;
        case Hour:
            back = timeinfo.tm_hour;
            break;
        case Minute:
            back = timeinfo.tm_min;
            break;
        case Second:
            back = timeinfo.tm_sec;
            break;
        case Week:
            back = daysOfTheWeek[timeinfo.tm_wday];
            break;
        default:
            break;
    }
    return back;
}

void MPython_NtpTime::setNtpTime(timeZone_t timeZone, String ntpServer){
    configTime(timeZone * 3600, 0, ntpServer.c_str());
#if defined(ARDUINO_ARCH_ESP8266)
    getLocalTime(&timeinfo, 5000);
#else
    getLocalTime(&timeinfo);
#endif
    
}
