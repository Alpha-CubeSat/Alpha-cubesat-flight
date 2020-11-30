#include "ACS.h"

ACS::ACS(int STBYpin, int STBZpin, int xPWMpin, int xout1, int xout2, int yPWMpin, int yout1, int yout2, int zout1, int zout2, int zPWMpin) : _STBYpin(STBYpin), _STBZpin(STBZpin),  _xPWMpin(xPWMpin), _xout1(xout1), _xout2(xout2), _yPWMpin(yPWMpin), _yout1(yout1), _yout2(yout2), _zout1(zout1), _zout2(zout2), _zPWMpin(zPWMpin) {
    Timer1.initialize(10000);
    Timer1.attachInterrupt(myISR);
    ACS::_rtObj.initialize();
}

void ACS::myISR(void){
    if (ACS::_pinset == 1){
        ACS::_pinset = 0;
    }
    else{
        ACS::_pinset = 1;
    }
    ACS::_permission = 0;
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

