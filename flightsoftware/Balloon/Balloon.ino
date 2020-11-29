#include "BurnWire.h"
#include "GPS.h"
#include "Camera.h"
#include "Photoresistor.h"

GPS gps = GPS(&Serial1, &Serial);
Camera cam = Camera(&Serial2, &Serial);
BurnWire burnWire = BurnWire(35, &Serial);
Photoresistor photoresistor = Photoresistor(17);

void setup(){
  gps.setup();
  cam.setup();
}

void loop(){
  if(!photoresistor.doorOpen()){
    Serial.println("CLOSED");
    burnWire.on();
  }
  else{
    Serial.println("OPEN");
    burnWire.off();
  } 
}
