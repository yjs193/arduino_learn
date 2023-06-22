#include "MPython_NtpTime.h"
#include <DFRobot_Iot.h>
DFRobot_Iot myIot;
MPython_NtpTime ntptime;

void setup()
{
    Serial.begin(9600);
    myIot.wifiConnect("dfrobotYanfa", "hidfrobot");
    while (!myIot.wifiStatus()) {yield();}
    Serial.println("Wifi connect success");
    ntptime.setNtpTime(ntptime.UTCEast8_t, "ntp.ntsc.ac.cn");
    //myIot.wifiDisconnect();
    Serial.println(ntptime.localTime(ntptime.Year));
}

void loop()
{
    delay(9000);
    Serial.println(ntptime.localTime(ntptime.Year));
}
