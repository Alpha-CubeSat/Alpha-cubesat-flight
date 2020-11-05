#include "BurnWire.h"

BurnWire::BurnWire(int p){
  this->pin= p;
}

String BurnWire::trigger(){
  pinMode(getPin(), OUTPUT);
  digitalWrite(getPin(), HIGH);
  return "Triggered burn wire";
}