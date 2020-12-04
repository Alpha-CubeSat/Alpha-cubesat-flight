#include "ACS.h"

ACS::ACS() : pinset(0), detuble(true), imu(IMU()) {
}

void ACS::setup(){
  //H-bridge standby pins
  pinMode(STBYpin, OUTPUT);  
  pinMode(STBZpin, OUTPUT);

  //Pins for X-Torquer
  pinMode(xPWMpin, OUTPUT);
  pinMode(xout1, OUTPUT);
  pinMode(xout2, OUTPUT);

  //Pins for Y-Torquer
  pinMode(yPWMpin, OUTPUT);
  pinMode(yout1, OUTPUT);
  pinMode(yout2, OUTPUT);

  //Pins for Z-Torquer
  pinMode(zout1, OUTPUT);
  pinMode(zout2, OUTPUT);
  pinMode(zPWMpin, OUTPUT);

  rtObj.initialize();
  imu.setup();
}

void ACS::ACSwrite(float current,  int out1,  int out2, int PWMpin) {
  if (current == 0.0) {
    digitalWrite(out1, 0);
    digitalWrite(out2, 0);
  }
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

void ACS::setCurrent(){
  rtObj.rtU.angularvelocity[0] = imu.getGyroX();
  rtObj.rtU.angularvelocity[1] = imu.getGyroY();
  rtObj.rtU.angularvelocity[2] = imu.getGyroZ();
  rtObj.rtU.Bfield_body[0] = imu.getMagX();
  rtObj.rtU.Bfield_body[1] = imu.getMagY();
  rtObj.rtU.Bfield_body[2] = imu.getMagZ();
  rtObj.step();

  if(detuble){
      current1 = rtObj.rtY.detuble[0];
      current2 = rtObj.rtY.detuble[1];
      current3 = rtObj.rtY.detuble[2];
  }
  if(!detuble){
      current1 = rtObj.rtY.point[0];
      current2 = rtObj.rtY.point[1];
      current3 = rtObj.rtY.point[2];
  }
}

void ACS::run(){
  ACSwrite(current1, xout1, xout2, xPWMpin);
  ACSwrite(current2, yout1, yout2, yPWMpin); //previously current1 for some odd reason
  ACSwrite(current3, zout1, zout2, zPWMpin);
}
