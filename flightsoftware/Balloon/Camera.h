#ifndef CAMERA_H
#define CAMERA_H

#include <HardwareSerial.h>
#include <SD.h>
#include <Adafruit_VC0706.h>

class Camera{
    public:
    Camera(HardwareSerial *ser);
    String setup();
    String takePhoto();
    Adafruit_VC0706 getAdaCam(){
        return adaCam;
    }

    private:
    Adafruit_VC0706 adaCam;
};

#endif