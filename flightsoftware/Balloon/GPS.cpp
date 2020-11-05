#include "GPS.h"

GPS::GPS(HardwareSerial *ser){
  this->serial = ser;
}

String GPS::setup(){
  getSerial().begin(9600);
  return "GPS is setup";
}

uint32_t GPS::getAltitude(){
  uint32_t latestAltitude = 0;
  if (getSerial().available() > 0){
    uint8_t GPSchar = getSerial().read();
    getTinyGPS().encode(GPSchar);
    latestAltitude = getTinyGPS().altitude.meters();
  }
  return latestAltitude;
}
