#include "DFRobot_GPS.h"


DFRobot_GPS::DFRobot_GPS() : enable(false), first(true)
{
}

DFRobot_GPS::~DFRobot_GPS()
{
#if defined NRF5
    if(serialH){
        uBit.serial.setRxBufferSize(MICROBIT_SERIAL_DEFAULT_BUFFER_SIZE);
    }
#endif
}

void DFRobot_GPS::begin(HardwareSerial *serial, int rx, int tx){
    serialH = serial;
    serialS= NULL;
#if defined NRF5
    rx = g_PinID[rx];
    tx = g_PinID[tx];
    MicroBitPin *rxpin = getMicroBitPin(rx);
    MicroBitPin *txpin = getMicroBitPin(tx);
    uBit.serial.redirect(txpin->name, rxpin->name);
    uBit.serial.setRxBufferSize(64);
    serialH->begin(9600);
#elif defined NRF52833
	rx = g_PinID[rx];
    tx = g_PinID[tx];
    MicroBitPin *rxpin = getMicroBitPin(rx);
    MicroBitPin *txpin = getMicroBitPin(tx);
	uBit.serial.redirect(*txpin, *rxpin);
    uBit.serial.setRxBufferSize(64);
    serialH->begin(9600);
#elif defined ESP_PLATFORM
    serialH->begin(9600,rx,tx);
#else
    serialH->begin(9600);
#endif
    setBaud(9600);
    //reset();
    //coldStart();
    closeOutputGPGLL();
    closeOutputGPVTG();
    closeOutputGPGSV();
    enable = true;
}


void DFRobot_GPS::begin(SoftwareSerial *serial){
    serialS=serial;
    serialH= NULL;
    serialS->begin(9600);
    setBaud(9600);
    //reset();
    //coldStart();
    closeOutputGPGLL();
    closeOutputGPVTG();
    closeOutputGPGSV();
    enable = true;
}

uint32_t DFRobot_GPS::getYear()
{
    getRawData();
    return gprmc_info.year;
}

uint32_t DFRobot_GPS::getMonth()
{
    getRawData();
    return gprmc_info.month;
}

uint32_t DFRobot_GPS::getDay()
{
    getRawData();
    return gprmc_info.day;
}

uint32_t DFRobot_GPS::getHour()
{
    getRawData();
    return gprmc_info.hour;
}

uint32_t DFRobot_GPS::getMinutes()
{
    getRawData();
    return gprmc_info.minutes;
}

uint32_t DFRobot_GPS::getSeconds()
{
    getRawData();
    return gprmc_info.seconds;
}

String DFRobot_GPS::getDateTime()
{
    String data = "";
    getRawData();
    String y = String(gprmc_info.year);
    String mon = gprmc_info.month < 10 ? (String("0") + String(gprmc_info.month)) : String(gprmc_info.month);
    String d = gprmc_info.day < 10 ? (String("0") + String(gprmc_info.day)) : String(gprmc_info.day);
    String h = gprmc_info.hour < 10 ? (String("0") + String(gprmc_info.hour)) : String(gprmc_info.hour);
    String min = gprmc_info.minutes < 10 ? (String("0") + String(gprmc_info.minutes)) : String(gprmc_info.minutes);
    String s = gprmc_info.seconds < 10 ? (String("0") + String(gprmc_info.seconds)) : String(gprmc_info.seconds);
    return data + y + "/" + mon + "/" + d + " " + h + ":" + min + ":" + s;
}

float DFRobot_GPS::getLatitude()
{
    getRawData();
    return gprmc_info.latitude;
}

float DFRobot_GPS::getLongitude()
{
    getRawData();
    return gprmc_info.longitude;
}

String DFRobot_GPS::getState()
{
    getRawData();
    if (gprmc_info.status == 'A') {
        return "A: valid data";
    }
    if (gprmc_info.status == 'V') {
        return "V: invalid data";
    }
    return String(gprmc_info.status);
}

String DFRobot_GPS::getLatitudeHemisphere()
{
    getRawData();
    if(gprmc_info.ns == 'N') {
        return String("N: Northern Hemisphere");
    }else if(gprmc_info.ns == 'S'){
        return String("S: Southern Hemisphere");
    }
    return String("Invalid data");
}

String DFRobot_GPS::getLongitudeHemisphere()
{
    getRawData();
    if(gprmc_info.ew == 'E') {
        return String("E: East longitude");
    }else if(gprmc_info.ew == 'W'){
        return String("W: West longitude");
    }
    return String("Invalid data");
}

float DFRobot_GPS::getGroundSpeed()
{
    getRawData();
    return gprmc_info.groundSpeed;
}

uint32_t DFRobot_GPS::getSatellitesCount()
{
    getRawData();
    return gpgga_info.satellitesCount;
}

float DFRobot_GPS::getAltitude()
{
    getRawData();
    return gpgga_info.msl;
}

float DFRobot_GPS::getPdop()
{
    getRawData();
    return gpgsa_info.pdop;
}

float DFRobot_GPS::getHdop()
{
    getRawData();
    return gpgsa_info.hdop;
}

float DFRobot_GPS::getVdop()
{
    getRawData();
    return gpgsa_info.vdop;
}

String DFRobot_GPS::getPositioningMode()
{
    getRawData();
    if (gpgsa_info.positioningMode == 1) {
        return String("Invalid targeting");
    } else if (gpgsa_info.positioningMode == 2) {
        return String("2D");
    } else if (gpgsa_info.positioningMode == 3) {
        return String("3D");
    }
    return String(gpgsa_info.positioningMode);
}

void DFRobot_GPS::getRawData(long timeOut)
{
    if(!enable) return;
    static long time = millis();
    if (millis() - time < 1000 && !first) {
        return;
    }
    String mess;
    String src[3] = {"none", "none", "none"};
    int data;
    long _time = millis();
    if(serialS && (serialS->available() || first))
    {
        if(first) {first = false;}
        while ((src[GPRMC] == "none" || src[GPGGA] == "none" || src[GPGSA] == "none") && millis() - _time < timeOut){
            if(serialS->read() == '$') {
                mess = "$";
                data = serialS->read();
                while(data!='\n' && data!='$'){
                    if(data >= 0) mess += (char)data;
                    data = serialS->read(); 
                    if(millis() - _time >= timeOut){
                        return;
                    }
                    yield();
                }
                mess += "\n";
                String protocol = mess.substring(1, 6);
                if (protocol == "GPRMC") {
                    if(checkGPS(mess)) src[GPRMC] = mess;
                } else if (protocol == "GPGGA"){
                    if(checkGPS(mess)) src[GPGGA] = mess;
                } else if (protocol == "GPGSA"){
                    if(checkGPS(mess)) src[GPGSA] = mess;
                }
                //break;
            }
            yield();
        }
    } else if(serialH && (serialH->available() || first)) {
        if(first) first = false;
        while ((src[GPRMC] == "none" || src[GPGGA] == "none" || src[GPGSA] == "none") && millis() - _time < timeOut){
            if(serialH->read() == '$') {
                mess = "$";
                data = serialH->read();
                while(data!='\n' && data!='$'){
                    if(data >= 0) mess += (char)data;
                    data = serialH->read();
                    if(millis() - _time >= timeOut){
                        return;
                    }
                    yield();
                }
                mess += "\n"; 
                String protocol = mess.substring(1, 6);
                if (protocol == "GPRMC") {
                    if(checkGPS(mess)) src[GPRMC] = mess;
                } else if (protocol == "GPGGA"){
                    if(checkGPS(mess)) src[GPGGA] = mess;
                } else if (protocol == "GPGSA"){
                    if(checkGPS(mess)) src[GPGSA] = mess;
                }
                //break;
            }
        }
        yield();
    }
    time = millis();
#if GPS_DEBUG
    Serial.println("-----------------");
    for(int i=0; i<3; i++)
    {
        if(src[i] != "none") Serial.print(src[i]);
        yield();
    }
#endif
    analyticalGPRMC(src[GPRMC]);
    analyticalGPGGA(src[GPGGA]);
    analyticalGPGSA(src[GPGSA]);
}

void DFRobot_GPS::analyticalGPGSA(String& body)
{
    String data[18];
    Split(body, data, 18);
    gpgsa_info.pdop = data[15].toFloat();
    gpgsa_info.hdop = data[16].toFloat();
    gpgsa_info.vdop = data[17].toFloat();
    gpgsa_info.positioningMode = data[2].toInt();
}

void DFRobot_GPS::analyticalGPGGA(String& body)
{
    String data[15];
    Split(body, data, 15);
    gpgga_info.satellitesCount = data[7].toInt();
    gpgga_info.hdop = data[8].toFloat();
    gpgga_info.msl = data[9].toFloat();
    gpgga_info.meanSeaLevel = data[11].toFloat();
}

void DFRobot_GPS::analyticalGPRMC(String& body)
{
    // 校验。。。
    String data[12];
    Split(body, data, 12);
    gprmc_info.latitude = positionConversion(data[5]);//经度
    gprmc_info.longitude = positionConversion(data[3]);//纬度
    gprmc_info.year = data[9].substring(4, 6).toInt() + 2000;
    gprmc_info.month = data[9].substring(2, 4).toInt();
    gprmc_info.day = data[9].substring(0, 2).toInt();
    gprmc_info.hour = (data[1].substring(0, 2).toInt() + 8) == 24 ? 0 : (data[1].substring(0, 2).toInt() + 8);
    gprmc_info.minutes = data[1].substring(2, 4).toInt();
    gprmc_info.seconds = data[1].substring(4, 6).toInt();
    gprmc_info.status = data[2][0];
    gprmc_info.ns = data[4][0];
    gprmc_info.ew = data[6][0];
    gprmc_info.groundSpeed = data[7].toFloat();
    gprmc_info.groundRoute = 9999;
    gprmc_info.magneticVariable = 9999;
    gprmc_info.modeIndication = data[11][0];
}

void DFRobot_GPS::reset()
{
    uint8_t cmd[] = {0xB5,0x62,0x06,0x04,0x04,0x00,0xFF,0x87,0x01,0x00,0x95,0xF7};
    wirteCmd(cmd, sizeof(cmd));
}

void DFRobot_GPS::coldStart()
{
    uint8_t cmd[] = {0xB5,0x62,0x06,0x04,0x04,0x00,0xFF,0xFF,0x02,0x00,0x0E,0x61};
    wirteCmd(cmd, sizeof(cmd));
}

void DFRobot_GPS::hotStart()
{
    uint8_t cmd[] = {0xB5,0x62,0x06,0x04,0x04,0x00,0x00,0x00,0x02,0x00,0x10,0x68};
    wirteCmd(cmd, sizeof(cmd));
}

void DFRobot_GPS::setBaud(uint32_t rate)
{
    if(rate == 9600){
        uint8_t cmd[] = {0xb5,0x62,0x06,0x00,0x14,0x00,0x01,0x00,0x00,0x00,0xd0,0x08,
            0x00,0x00,0x80,0x25,0x00,0x00,0x07,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0xa6,0xcd,
            0xb5,0x62,0x06,0x00,0x01,0x00,0x01,0x08,0x22};
        wirteCmd(cmd, sizeof(cmd));
    }
}

void DFRobot_GPS::closeOutputGPGLL()
{
    uint8_t cmd[] = {0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x4c,0x4c,0x2a,0x32,
        0x31,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x01,0x00,0xfb,0x11};
    wirteCmd(cmd, sizeof(cmd));
}

void DFRobot_GPS::closeOutputGPVTG()
{
    uint8_t cmd[] = {0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x56,0x54,0x47,0x2a,0x32,
        0x33,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x05,0x00,0xff,0x19};
    wirteCmd(cmd, sizeof(cmd));
}

void DFRobot_GPS::closeOutputGPGSV()
{
    uint8_t cmd[] = {0x24,0x45,0x49,0x47,0x50,0x51,0x2c,0x47,0x53,0x56,0x2a,0x32,
    0x34,0x0d,0x0a,0xb5,0x62,0x06,0x01,0x03,0x00,0xf0,0x03,0x00,0xfd,0x15};
    wirteCmd(cmd, sizeof(cmd));
}

void DFRobot_GPS::wirteCmd(uint8_t *cmd, uint32_t len)
{
    if(serialS){
        serialS->write(cmd, len);
    }else if(serialH){
        serialH->write(cmd, len);
    }
}

float DFRobot_GPS::positionConversion(String& body)
{
  String buf[2];
  Split(body, buf, 2, '.');
  float fghi = buf[1].toInt();
  float de = buf[0].substring(buf[0].length() - 2, buf[0].length()).toInt();
  float abc = buf[0].substring(0, buf[0].length() - 2).toInt();
  return abc + (de/60) + (fghi/600000);
} 

void DFRobot_GPS::Split(String& body, String data[], int len, char separator)
{
    int head = 0, tail = 0;
    for(int i=0; i<len; i++){
        for(int j=head; j<body.length(); j++){
            if(j==body.length()-1){
                tail = j+1;
                data[i] = body.substring(head, tail);
            } else if (body[j] == separator) {
                tail = j;
                data[i] = body.substring(head, tail);
                head = j+1;
                break;
            }
            yield();
        }
        yield();
    }
}

bool DFRobot_GPS::checkGPS(String& body)
{
    char ch=body.charAt(1);
    int x=(int)ch;
    int y;
    String checkCode="";
    for(int i=2;i<body.length();i++){
        y=(int)body.charAt(i);
        if(y == (int)'*') {
            checkCode = body.substring(i+1, body.length()-2);
            break;
        }
        x=x^y;
    }
    String check = String(x,HEX);
    check.toUpperCase();
    int length = checkCode.length() - check.length();
    for(int i=0; i<length; i++) {
        check = "0"+check;
    }
    return check == checkCode;
}



