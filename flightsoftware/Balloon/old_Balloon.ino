/*#include <TinyGPS++.h>                           //from http://arduiniana.org/libraries/tinygpsplus/
#include <Adafruit_VC0706.h>
#include <SD.h>

#define GPSserial Serial1
#define chipSelect BUILTIN_SDCARD

TinyGPSPlus gps;                                 
uint32_t latestAltitude = 0;
uint8_t GPSchar;
Adafruit_VC0706 cam = Adafruit_VC0706(&Serial2);

void setup(){
  GPSserial.begin(9600);
  Serial.begin(115200);
  camSetup();
  cBuf = commandBuffer();
}

void loop(){
  
  doorOpen();
  //Serial.println(getAltitude());
  //triggerBurnWire();
}

uint32_t getAltitude(){
  if (GPSserial.available() > 0){
    GPSchar = GPSserial.read();
    gps.encode(GPSchar);
    latestAltitude = gps.altitude.meters();
  }
  return latestAltitude;
}

boolean doorOpen() { 
  int lightVal = analogRead(17);

  if(lightVal > 400){
    Serial.println("Door open");
    return true;
  }
  else{
    Serial.println("Door closed");
    takePhoto();
    return false;
  }
}

void triggerBurnWire() {
  pinMode(35, OUTPUT);
  digitalWrite(35, HIGH);
}

void camSetup(){
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println("Card failed, or not present");
  }
  if (!cam.begin()) {
    Serial.println("Camera not found");
  }
  cam.setImageSize(VC0706_640x480);  
}

void takePhoto(){
  if (!cam.takePicture()){ 
    Serial.println("Failed to snap!");
  }
  else{
    Serial.println("Photo taken, saving image");
    // Create an image with the name IMAGExx.JPG
    char filename[13];
    strcpy(filename, "IMAGE00.JPG");
    for (int i = 0; i < 100; i++) {
      filename[5] = '0' + i/10;
      filename[6] = '0' + i%10;
      // create if does not exist, do not open existing, write, sync after write
      if (! SD.exists(filename)) {
        break;
      }
    }
    
    // Open the file for writing
    File imgFile = SD.open(filename, FILE_WRITE);
  
    // Get the size of the image (frame) taken  
    uint16_t jpglen = cam.frameLength();
  
    // Read all the data up to # bytes!
    byte wCount = 0; // For counting # of writes
    while (jpglen > 0) {
      // read 32 bytes at a time;
      uint8_t *buffer;
      uint8_t bytesToRead = min(64, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
      buffer = cam.readPicture(bytesToRead);
      imgFile.write(buffer, bytesToRead);
      if(++wCount >= 64) { // Every 2K, give a little feedback so it doesn't appear locked up
        Serial.print('.');
        wCount = 0;
      }
      jpglen -= bytesToRead;
    }
    imgFile.close();
    Serial.println("Image saved");
  }
}*/
