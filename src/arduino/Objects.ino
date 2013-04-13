Debug debug;
DogRPC raspi;

unsigned short lightsSignature[12] = {LIGHTS_SIGNATURE,0,0,0,2};
EtekcitySwitch lights(RF_TRANSMITTER_PIN, lightsSignature, LIGHTS_PERIOD_DURATION);

EasyButton lightsBtn(LIGHT_BTN_PIN, lightsDoorwayButton, CALL_IN_PUSH, true); // true is for pullup resistor

unsigned short lightStatus;
// lightStatus is an 3-bit number where light 1 = 001, 2 = 010, 3 = 100 and if there's more than one 
// light referenced in the pattern they are ANDed (eg. lights 1 and 2 on, 3 off would be 110).
// Behaviors:
//   lightStatus & bitmask == bitmask: check if all the lights that are on in bitmask are on in lightStatus
//   lightStatus | bitmask == bitmask: check if all the lights that are off in bitmask are off in lightStatus
//   lightStatus |= bitmask: makes all values set to 1 in bitmask set to 1 in lightStatus, leaves the rest alone
//   lightStatus &= bitmask: makes all values set to 0 in bitmask set to 0 in lightStatus, leaves the rest alone
//   lightStatus ^ desiredLightStatus: generates a bitmask of lights to be toggled to get to desiredLightStatus

volatile unsigned long previousLightSignal = 0;
volatile unsigned long previousLightTime = 0;
volatile unsigned long queuedPattern = 0;
