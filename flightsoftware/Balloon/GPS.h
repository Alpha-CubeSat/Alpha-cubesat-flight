#ifndef GPS_H
#define GPS_H

#include <HardwareSerial.h>
#include <Wire.h>
#include <TinyGPS++.h>    

class GPS{
    public:
    GPS(HardwareSerial *ser);
    String setup();
    uint32_t getAltitude();
    HardwareSerial getSerial(){
        return *serial;
    }
    TinyGPSPlus getTinyGPS(){
        return gps;
    }

    private:
    HardwareSerial *serial;
    TinyGPSPlus gps;  
};

#endif