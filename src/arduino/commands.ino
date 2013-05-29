/**************************
* Test and Debug Commands *
**************************/

void testCmd(Vector<char*> args) {
  String argsStr = "";
  for (unsigned int i = 0; i < args.size(); i++) {
      argsStr += args[i];
      if (i < args.size()-1) argsStr += "`, `"; // don't add this the last time
  }
  char argsChars[argsStr.length()+1];
  argsStr.toCharArray(argsChars, argsStr.length()+1);
  
  debug.info("Recieved test command with args: `%s`", argsChars);
}

void dummyCmd(unsigned long receivedPattern, unsigned int period) {
    digitalWrite(DEBUG_LED, !digitalRead(DEBUG_LED)); // toggle DEBUG_LED
}

/*************************
* Status Screen commands *
*************************/
void statusRequestCmd(Vector<char*> args) {
  if (strstr(args[0], "lights") != NULL) { // Request for the status of the lights
    sendLightsStatus();
  }
}

/******************
* Lights Commands *
******************/

void lightsCmd(Vector<char*> args) {
  int lightsCode = 0;
  if (strstr(args[0], "on") != NULL) lightsCode |= 0b001;
  if (strstr(args[1], "on") != NULL) lightsCode |= 0b010;
  if (strstr(args[2], "on") != NULL) lightsCode |= 0b100;
  lightsCode &= ENABLED_LIGHTS; // Leave disabled lights alone
  
  lights.sendSignal(lightsCode);
  lightStatus ^= lightsCode; // a = a^b toggles bits in a that correspond to a 1 in b, leaves the others alone
}

void lightsDoorwayButton() {
  if (lightStatus == 0) { // if lights are off
    lights.sendSignal(ENABLED_LIGHTS); // turn them all on (except disabled)
    lightStatus = ENABLED_LIGHTS; // theoretically enabled lights all are on now
  } else { // if lights are on
    lights.sendSignal(lightStatus); // turn off all the ones that are on
    lightStatus = 0b000; // theoretically all are off now
  }
}

//  IMPORTANT: Don't try to print anything in this function. It's called by an interrupt, so it will
//  make the code crash
void rfSignalRecievedCmd(unsigned long receivedPattern, unsigned int period) {
   // Send light signal only if it's NOT a repeat or enough time has elapsed
  if (millis() - previousLightTime > MAX_TIME_BEFORE_REPEAT_PULSE || receivedPattern != previousLightSignal) {
    queuedPattern = receivedPattern; // Register this code to be processed after the interrupt ends
  }
  previousLightTime = millis();
  previousLightSignal = receivedPattern;
}

void lightStatusCorrectionCmd(Vector<char*> args) {
  lightStatus = 0;
  if (strstr(args[0], "on") != NULL) lightStatus |= 0b001;
  if (strstr(args[1], "on") != NULL) lightStatus |= 0b010;
  if (strstr(args[2], "on") != NULL) lightStatus |= 0b100;
  debug.info("Light status corrected, new status: %s, %s, %s", BITWISE_STATUS(lightStatus));
}

/********************
* Calendar Commands *
********************/

void calInfoCmd(Vector<char*> args) {
  if (strstr(args[0], "will_nextevent_millis") != NULL) {
    nextEventTime_Will = millis() + int(args[1]);
  }
}
