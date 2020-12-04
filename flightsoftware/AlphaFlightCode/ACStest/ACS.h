#ifndef ACS_H
#define ACS_H

#include "StarshotACS0.h"
#include <Arduino.h>
#include <TimerOne.h>
#include "Constants.h"
#include "IMU.h"

class ACS{

    public:
    ACS();
    
    void myISR(void);
    void setup();
    void run();
    void setCurrent();
    void ACSwrite(int torqorder,  float current,  int out1,  int out2, int PWMpin);
    int getPinset(){
        return pinset;
    }
    void setPinset(int val){
        pinset = val;
    }


    private:
    StarshotACS0ModelClass rtObj;
    int pinset;

    //0 = dampening/detumble, 1 = PD controller/pointing
    boolean detuble;

    IMU imu;
    float current1;
    float current2;
    float current3;

};

#endif