#ifndef BurnWire_H
#define BurnWire_H

#include <Arduino.h> 

class BurnWire{
    public:
    BurnWire(int pin);
    String trigger();
    int getPin(){
        return pin;
    }

    private:
    int pin;  
};

#endif