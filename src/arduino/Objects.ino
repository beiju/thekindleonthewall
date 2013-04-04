Debug debug;
DogRPC raspi;
EasyButton lightsBtn(LIGHT_BTN_PIN, lightsBtnCmd, CALL_IN_PUSH, true); // true is for pullup resistor

int lightStatus;
unsigned long previousLightSignal = 0;
unsigned long previousLightTime = 0;
