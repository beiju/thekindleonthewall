#import "Transciever.h"

#define DELAYSHORT 160 // These may need to be parameters
#define DELAYLONG  500

void Transciever::begin(int signature, int termination, byte pin) {
  _signature = signature;
  _termination = termination;
  _pin = pin;
  pinMode(pin, OUTPUT);
}

void Transciever::ookPulse(int on, int off) {
  digitalWrite(_pin, HIGH);
  delayMicroseconds(on);
  digitalWrite(_pin, LOW);
  delayMicroseconds(off);
}

void Transciever::send(uint8_t command) {
  byte i, k;
  // send 16 times
  for(k=0;k<16;k++) {
    // send signature first
    for(i=0;i<16;i++) {
      if((_signature>>(15-i)) & 0x1) {
        ookPulse(DELAYLONG, DELAYSHORT);
      } 
      else {
        ookPulse(DELAYSHORT, DELAYLONG);
      }
    }
    for(i=0;i<8;i++) {
      if((command>>(7-i)) & 0x1) {
        ookPulse(DELAYLONG, DELAYSHORT);
      } 
      else {
        ookPulse(DELAYSHORT, DELAYLONG);
      }
    }

    // end with a '010'
    ookPulse(DELAYSHORT, DELAYLONG);
    ookPulse(DELAYLONG, DELAYSHORT);
    ookPulse(DELAYSHORT, DELAYLONG);

    // short delay
    delay(5);
  }
}

//! needs a Transciever::recieve() or some such
