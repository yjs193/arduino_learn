#include "DFRobot_SD.h"

DFRobot_SD file;

void setup()
{
    Serial.begin(9600);
    while (!Serial);
    if (!file.begin(3))
    {
        Serial.println("SD card connection failed!");
        return;
    }
    Serial.println("Delete the old file test.txt");
    file.deleteFile("test.txt");
    Serial.println("Deleted successfully\n");
    Serial.println("Create a new file named test.txt and write to it");
    file.writeText("test.txt", "line1\nline2\nline3\nline4\nline5\n", false, false);
    Serial.println("Created successfully\n");
    Serial.println("Read the entire contents of the file:");
    Serial.println(file.readAllText("test.txt"));
    Serial.print("Read the third line contents of the file: ");
    Serial.println(file.readLine("test.txt", 3));
    Serial.print("\nGet the name of the file: ");
    Serial.println(file.getFileInfo("test.txt", file.eNAME));
    Serial.print("\nGet the path of the file: ");
    Serial.println(file.getFileInfo("test.txt", file.ePATH));
    Serial.print("\nGet the type of the file: ");
    Serial.println(file.getFileInfo("test.txt", file.eTYPE));
    Serial.print("\nGet the number of lines in the file: ");
    Serial.println(file.getFileInfo("test.txt", file.eLINES));
    Serial.print("\nGet the size of the file: ");
    Serial.println(file.getFileInfo("test.txt", file.eSIZE));
}

void loop() {}
