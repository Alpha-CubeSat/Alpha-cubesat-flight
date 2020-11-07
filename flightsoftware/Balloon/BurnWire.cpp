#include "BurnWire.h"

BurnWire::BurnWire(int p, usb_serial_class *swSer){
  this->pin= p;
  this->softwareSerial = swSer;
}

void BurnWire::trigger(){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  getSoftwareSerial().println("Triggered burn wire");
}