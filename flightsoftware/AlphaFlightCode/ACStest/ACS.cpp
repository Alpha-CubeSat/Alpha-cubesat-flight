#include "ACS.h"

ACS::ACS() : _pinset(0), _permission(0), _ACSmode(0) {
    Timer1.initialize(10000);
    Timer1.attachInterrupt(myISR);
    ACS::_rtObj.initialize();
}

void ACS::myISR(void){
  if (getPinset() == 1){
    setPinset(0);
  }
  else{
    setPinset(1);
  }
  setPermission(0);
}

void ACS::ACSwrite(int torqorder,  float current,  int out1,  int out2, int PWMpin) {
  if (current == 0.0) {
    digitalWrite(out1, 0);
    digitalWrite(out2, 0);
  }
  if (torqorder == 0) {
    if (current > 0) {
      digitalWrite(out1, 1);
      digitalWrite(out2, 0);
      analogWrite(PWMpin, current);
    }
    if (current < 0) {
      digitalWrite(out1, 0);
      digitalWrite(out2, 1);
      analogWrite(PWMpin, current);
    }
  }
  if (torqorder == 1) {
    if (current > 0) {
      digitalWrite(out1, 0);
      digitalWrite(out2, 1);
      analogWrite(PWMpin, current);
    }
    if (current < 0) {
      digitalWrite(out1, 1);
      digitalWrite(out2, 0);
      analogWrite(PWMpin, current);
    }
  }
}

