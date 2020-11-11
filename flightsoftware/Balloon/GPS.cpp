#include "GPS.h"

GPS::GPS(HardwareSerial *hwSer, usb_serial_class *swSer){
  hardwareSerial = hwSer;
  softwareSerial = swSer;
}

void GPS::setup(){
  getHardwareSerial().begin(9600);
  getSoftwareSerial().println("GPS is setup");
}

uint32_t GPS::getAltitude(){
  uint32_t latestAltitude = 0;
  if (getHardwareSerial().available() > 0){
    uint8_t GPSchar = getHardwareSerial().read();
    gps.encode(GPSchar);
    latestAltitude = gps.altitude.meters();
  }
  return latestAltitude;
}
