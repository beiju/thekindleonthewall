#include "EtekcitySwitch.h"

/************
* EtekcitySwitch
************/

/* periodusec and reps have defaults in the .h */
EtekcitySwitch::EtekcitySwitch(unsigned short pin, unsigned short* signature, unsigned int periodusec) : 
    RemoteSwitch(pin,periodusec,3) {
  //Call contructor
  _signature = signature;
}

// Unsigned short is 4 bits long, but should only be 
void EtekcitySwitch::sendSignal(unsigned short signal) {    
  sendTelegram(getTelegram(signal), _pin);
}

/* Get the data that sendTelegram expects ({S,S,S,S,S,S,S,S,s,s,s,2} where S=signature and s=signal) */
unsigned long EtekcitySwitch::getTelegram(unsigned short signal) {
  unsigned short trits[12];
  memcpy(_signature, trits, sizeof(trits));
  
  // The formula for this is: trits[i+8] = (signal & (0b001 << i)) ? 1 : 0;
  trits[8] = (signal & 0b001) ? 1 : 0;
  trits[9] = (signal & 0b010) ? 1 : 0;
  trits[10] = (signal & 0b100) ? 1 : 0;

  return encodeTelegram(trits);
}

unsigned short EtekcitySwitch::signalFromPattern(unsigned long pattern) {
  unsigned short signal = 0;
  for (unsigned short i = 0; i < 3; i++) {
    pattern /= 3; // happens before comparison to get rid of the 2 in the ones digit
    if (pattern % 3 != 0) signal |= (2^i); // if the value isn't 0, put it in the appropriate bit
  }
  return signal;
}
