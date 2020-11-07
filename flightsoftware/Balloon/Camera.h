#ifndef CAMERA_H
#define CAMERA_H

#include <HardwareSerial.h>
#include <SD.h>
#include <usb_serial.h>
#include <Adafruit_VC0706.h>

class Camera{
    public:
    Camera(HardwareSerial *ser, usb_serial_class *swSer);
    void setup();
    void takePhoto();
    usb_serial_class getSoftwareSerial(){
        return *softwareSerial;
    }
    HardwareSerial getHardwareSerial(){
        return *hardwareSerial;
    }
    Adafruit_VC0706 getAdaCam(){
        return *adaCam;
    }
    
    private:
    HardwareSerial *hardwareSerial;
    usb_serial_class *softwareSerial;
    Adafruit_VC0706 *adaCam;    
};

#endif