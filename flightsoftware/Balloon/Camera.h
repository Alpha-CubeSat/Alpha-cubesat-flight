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
    HardwareSerial getHwSerial(){
        return *hwSerial;
    }
    Adafruit_VC0706 getAdaCam(){
        return adaCam;
    }

    private:
    HardwareSerial *hwSerial;
    Adafruit_VC0706 adaCam;
};

#endif