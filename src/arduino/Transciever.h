#ifndef Transciever_h
#define Transciever_h

#include "Arduino.h"

class Transciever {
  public:
    void begin(int, int, byte);
    void send(uint8_t);
  
  private:
    int _signature;
    int _termination;
    int _pin;
    
    void ookPulse(int, int);
};

#endif
