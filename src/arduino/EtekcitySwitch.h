#ifndef EtekcitySwitch_h
#define EtekcitySwitch_h

#include "RemoteSwitch.h"

/**
* EtekcitySwitch simulates a remote, as sold by Etekcity. These remotes have 3 buttons to toggle each light.
*/
class EtekcitySwitch: RemoteSwitch {
  public:
    /**
    * Constructor
    *
    * @param pin    output pin on Arduino to which the transmitter is connected
    * @param periodsec  Duration of one period, in microseconds.
    * @see RemoteSwitch
    */
    EtekcitySwitch(unsigned short pin, unsigned short* signature, unsigned int periodusec=260);
    
    /**
    * Send a on or off signal to a device.
    *    
    * @param signal  Device to switch. Range: [1..8]. Value: 0bABC where A,B,C = 1 if the corresponding device 
    *                should be switched
    */
    void sendSignal(unsigned short signal);
    
    /**
    * @see RemoteSwitch::getTelegram
    */
    unsigned long getTelegram(unsigned short device);
    
    /** 
    * Extract the signal from a pattern returned from RemoteReciever. NOT safe to use within interrupts. 
    * Modifies the input.
    *
    * @param pattern  A pattern in the format returned from RemoteReciever (NOTE: This will be modified)
    */
    static unsigned short signalFromPattern(unsigned long pattern);
    
  private: 
    unsigned short* _signature;
    unsigned int _msgTemplate;
};

#endif
