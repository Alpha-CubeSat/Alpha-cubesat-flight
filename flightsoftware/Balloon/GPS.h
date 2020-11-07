#ifndef GPS_H
#define GPS_H

#include <HardwareSerial.h>
#include <Wire.h>
#include <TinyGPS++.h>    
#include <usb_serial.h>

class GPS{
    public:
    GPS(HardwareSerial *hwSer, usb_serial_class *swSer);
    void setup();
    uint32_t getAltitude();
    HardwareSerial getHardwareSerial(){
        return *hardwareSerial;
    }
    usb_serial_class getSoftwareSerial(){
        return *softwareSerial;
    }

    private:
    HardwareSerial *hardwareSerial;
    usb_serial_class *softwareSerial;
    TinyGPSPlus gps;  
};

#endif