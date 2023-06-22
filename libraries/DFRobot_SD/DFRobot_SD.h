#ifndef DFROBOT_SD_H
#define DFROBOT_SD_H

#include <SD.h>
#include <SPI.h>


class DFRobot_SD
{
public:
    DFRobot_SD();
    typedef enum {eNAME, ePATH, eTYPE, eLINES, eSIZE}FileInfo;
    bool begin(int cspin);
    String readLine(String filename, uint32_t line);
    String readAllText(String filename);
    void writeText(String filename, String text, bool append, bool linebreak);
    void deleteFile(String filename);
    String getFileInfo(String filename, FileInfo type);
    void ls(uint32_t baud = 9600);
private:
    Sd2Card card;
    SdVolume volume;
    SdFile root;
    bool _begin;
    int _cspin;
};
#endif
