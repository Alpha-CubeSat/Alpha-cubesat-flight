#ifndef IridiumConnection_H
#define IridiumConnection_H

#include <HardwareSerial.h>
#include <Wire.h>
#include <usb_serial.h>

class IridiumConnection{
    public:
    IridiumConnection(int p1, int p2);
    HardwareSerial getHardwareSerial(){
        return *hardwareSerial;
    }
    usb_serial_class getSoftwareSerial(){
        return *softwareSerial;
    }

    private:
     
};

#endif