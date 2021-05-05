#include <SoftwareSerial.h>
#include <Wire.h>
#include <DS3231.h>
DS3231  rtc(SDA, SCL);
unsigned long timestamp;
#include <AltSoftSerial.h>
SoftwareSerial  HM10(8, 9);
#include <SPI.h>
#include <SD.h>
Sd2Card card;
File myFile;
int incomingByte = 0;
const int chipSelect = 4;
const byte numChars = 64;
char receivedChars[numChars];   
boolean newData = false;
void setup()
{
  Serial.begin(9600);
  HM10.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  BTsend();
  rtc.begin();
}

void loop()
{
  recvWithEndMarker();
  showNewData();
  if (millis() - timestamp > 750) {

    HM10.write("AT+DISI?");
    timestamp = millis();
  }
}

void BTsend()
{
  HM10.write("AT+DISI?");
  HM10.read();
}
void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = ':';
  char rc;
  while (HM10.available() > 0 && newData == false) {
    rc = HM10.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    Serial.println(receivedChars);
    int RSSI1;
    int minrssi;
     char UUID_ARRAY[36];
    if (receivedChars[0] == 'F')
    {
     
      for (int i = 0; i <= 31; i++)
      {
        UUID_ARRAY[i] = receivedChars[i];
      }
    
      
    } 
     if (receivedChars[0] == '-')
    {
      Serial.println(UUID_ARRAY);
      char RSSI_ARRAY[6] = {receivedChars[1], receivedChars[2], receivedChars[3]};
      String StringRSSI = RSSI_ARRAY;
      RSSI1 = StringRSSI.toInt();
      int RSSI2 = -1*RSSI1;
      minrssi = -80;
      
      if (RSSI1 > minrssi)
      {

        Serial.println("This is the RSSI:");
        Serial.println(RSSI2);
        Serial.print(rtc.getDateStr());
        Serial.print(",");
        Serial.print(rtc.getTimeStr());
        Serial.print(",");
        Serial.println(UUID_ARRAY);
        SDsave(UUID_ARRAY,RSSI1);
    }
      }
    }
  
  newData = false;
}

void SDsave(String StringUUID,int RSSI2)
{
  myFile = SD.open("RSSI92.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print(rtc.getDateStr());
    myFile.print(",");
    myFile.print(rtc.getTimeStr());
    myFile.print(",");
    myFile.print(StringUUID);
    myFile.print(",");
    myFile.println(RSSI2);
    myFile.close();
    
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}
