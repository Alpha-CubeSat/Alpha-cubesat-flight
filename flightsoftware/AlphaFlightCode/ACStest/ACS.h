#ifndef ACS_H
#define ACS_H

#include <StarshotACS0.h>
#include <Arduino.h>
#include <TimerOne.h>
#include "Constants.h"

class ACS{

    public:
    ACS();
    
    void myISR(void);
    void ACSwrite(int torqorder,  float current,  int out1,  int out2, int PWMpin);
    int getPinset(){
        return _pinset;
    }
    void setPinset(int val){
        _pinset = val;
    }
    void setPermission(int val){
        _permission = val;
    }

    private:
    
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
    int _pinset;
    int _permission;
    int _ACSmode;

};

#endif