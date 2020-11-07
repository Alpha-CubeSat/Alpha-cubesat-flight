#include "BurnWire.h"
#include "GPS.h"
#include "Camera.h"

GPS gps = GPS(&Serial1, &Serial);
Camera cam = Camera(&Serial2, &Serial);
BurnWire burnWire = BurnWire(35, &Serial);

void setup(){
  Serial.begin(115200);
  gps.setup();
  //cam.setup();
  

}

void loop(){
  //Serial.println(gps.getAltitude());
  //burnWire.trigger(); 
}
