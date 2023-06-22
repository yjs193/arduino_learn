#include "DFRobot_SD.h"

DFRobot_SD::DFRobot_SD()
{
    _begin = false;
    _cspin = 10;
}

bool DFRobot_SD::begin(int cspin)
{
    _cspin = cspin;
    return _begin = SD.begin(cspin);
}

String DFRobot_SD::readLine(String filename, uint32_t line)
{
    if (!_begin) if (!this->begin(_cspin)) return "<mind+err:Initialization failed>";
    if (!SD.exists(filename)) return "<mind+err:File not found>";
    File myFile = SD.open(filename);
    if (myFile.isDirectory()){
        myFile.close();
        return "<mind+err:File type error>";
    }
    String ret = "";
    uint32_t count = 0;
    int rd;
    while (myFile.available()){
        rd = myFile.read();
        if (rd != '\n')
            ret += (char)rd;
        else{
            count++;
            if (count == line){
                myFile.close();
                return ret;
            }
            else
                ret = "";
        }
    }
    myFile.close();
    if (ret != ""){
        count++;
        return count == line ? ret : "<mind+err:Out of range>";
    }
    else
        return "<mind+err:Out of range>";
}

String DFRobot_SD::readAllText(String filename)
{
    if (!_begin) if (!this->begin(_cspin)) return "<mind+err:Initialization failed>";
    if (!SD.exists(filename)) return "<mind+err:File not found>";
    File myFile = SD.open(filename);
    if (myFile.isDirectory()){
        myFile.close();
        return "<mind+err:File type error>";
    }
    String ret = "";
    while (myFile.available())
        ret += (char)myFile.read();
    myFile.close();
    return ret;
}

void DFRobot_SD::writeText(String filename, String text, bool append, bool linebreak)
{
    if (!_begin) if (!this->begin(_cspin)) return;
    File myFile;
    if (SD.exists(filename)){
        myFile = SD.open(filename);
        if (myFile.isDirectory()){
            Serial.println("The specified file name is the same as the existing folder. Please specify another name.");
            myFile.close();
            return;
        }
        myFile.close();
        if (!append)
            SD.remove(filename);
    }
    myFile = SD.open(filename, FILE_WRITE);
    if (linebreak) 
        myFile.println(text);else myFile.print(text);
    myFile.close();
}

void DFRobot_SD::deleteFile(String filename)
{
    if (!_begin) if (!this->begin(_cspin)) return;
    if (SD.exists(filename)) 
        SD.remove(filename);
}

String DFRobot_SD::getFileInfo(String filename, FileInfo type)
{
    if (!_begin) if (!this->begin(_cspin)) return "<mind+err:Initialization failed>";
    if (!SD.exists(filename)) 
        return "<mind+err:File not found>";
    File myFile = SD.open(filename);
    if (myFile.isDirectory()){
        myFile.close();
        return "<mind+err:File type error>";
    }
    String info;
    switch (type){
        case eNAME: {
            info = filename.substring(filename.lastIndexOf("/") + 1);
            break;
        }
        case ePATH: {
            int p = filename.lastIndexOf("/");
            if (p > 0)
                info = filename.startsWith("/") ? filename.substring(0, p) : "/" + filename.substring(0, p);
            else
                info = "/";
            break;
        }
        case eTYPE: {
            int p = filename.lastIndexOf("/");
            String sub = p > 0 ? filename.substring(p) : filename;
            int d = sub.lastIndexOf(".");
            info = d > 0 ? sub.substring(d) : "No extension";
            break;
        }
        case eLINES: {
            uint32_t count = 0, flag = 0;
            int rd;
            while (myFile.available()){
                rd = myFile.read();
                if (rd == '\n'){
                    count++;
                    flag = 0;
                }
                else
                    flag++;
            }
            if (flag != 0) count++;
            info = (String)count;
            break;
        }
        case eSIZE: {
            info = (String)myFile.size();
            break;
        }
    }
    myFile.close();
    return info;
}

void DFRobot_SD::ls(uint32_t baud)
{
    if (!_begin)if (!this->begin(_cspin)) return;
    if (Serial) Serial.flush();
    Serial.begin(baud);
    card.init(SPI_HALF_SPEED, _cspin);
    volume.init(card);
    root.openRoot(volume);
    root.ls(LS_R | LS_DATE | LS_SIZE);
}
