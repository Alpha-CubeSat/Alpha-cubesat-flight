#ifndef ACS_H
#define ACS_H

#include <StarshotACS0.h>
#include <Arduino.h>

class ACS{

    public:
    ACS(int STBYpin, int STBZpin, int xPWMpin, int xout1, int xout2, int yPWMpin, int yout1, int yout2, int zout1, int zout2, int zPWMpin);
    void myISR(void);
    void ACSwrite(int torqorder,  float current,  int out1,  int out2, int PWMpin);

    private:
    int _ACSmode = 0;
    int _STBYpin;
    int _STBZpin;
    int _xPWMpin;
    int _xout1;
    int _xout2;
    int _yPWMpin;
    int _yout1;
    int _yout2;
    int _zout1;
    int _zout2;
    int _zPWMpin;
    StarshotACS0ModelClass _rtObj;
    int _pinset = 0;
    int _permission = 0;

};

#endif