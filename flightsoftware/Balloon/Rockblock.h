#ifndef Rockblock_H
#define Rockblock_H

#include <HardwareSerial.h>
#include <usb_serial.h>
#include <IridiumSBD.h>

class Rockblock{
    public:
    Rockblock(HardwareSerial *hwSerial, usb_serial_class *swSerial);

    usb_serial_class getSoftwareSerial(){
        return *softwareSerial;
    }

    private:
    usb_serial_class *softwareSerial;
     
};

#endif