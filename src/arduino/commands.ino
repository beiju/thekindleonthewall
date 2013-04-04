void lightsCmd(Vector<char*> args) {
  // Pattern: 00BBCCAA00 where A, B, C is the on/off state of lights 1, 2, and 3 (11 = on, 00 = off)
  int lightsCode = 0;
  if (strstr(args[0], "on") != NULL) lightsCode &= 0b001;
  if (strstr(args[1], "on") != NULL) lightsCode &= 0b010;
  if (strstr(args[2], "on") != NULL) lightsCode &= 0b100;
}

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
//  debug.debug("Command run");
    if (digitalRead(13) == LOW) digitalWrite(13, HIGH); else digitalWrite(13, LOW);
}

void lightsBtnCmd() {
  if (lightStatus == 0) { // if lights are off
    sendLightsCode(0b011); // turn them all on (except 3--disabled)
    lightStatus = 0b011; // theoretically all are on now (except 3--disabled)
  } else { // if lights are on
    sendLightsCode(lightStatus); // turn off all the ones that are on
    lightStatus = 0b000; // theoretically all are off now
  }
}

//  IMPORTANT: Don't try to print anything in this function. It's called by an interrupt, so it will
//  make the code crash
void rfSignalRecievedCmd(unsigned long receivedPattern, unsigned int period) {
  // lightsFromPattern returns an 3-bit number where light 1 = 001, 2 = 010, 3 = 100 and if there's
  // more than one light referenced in the pattern they are ANDed (lights 1&2 = 110, etc).
  // lightStatus is stored in the same way. XORing them together returns a number with each bit swapped
  // if the bit is ON IN recievedLights, leaves it alone otherwise
  digitalWrite(13, !digitalRead(13));
  if (millis() - previousLightTime > MAX_TIME_BEFORE_REPEAT_PULSE || receivedPattern != previousLightSignal) {
    // Only if it's not a repeat or enough time has elapsed 
    lightStatus = lightStatus ^ lightsFromPattern(receivedPattern, true); // true suppresses print statement
  }
  previousLightTime = millis();
  previousLightSignal = receivedPattern;
}

void lightStatusCorrectionCmd(Vector<char*> args) {
  if (strstr(args[0], "on") != NULL) lightStatus |= 0b001;
  if (strstr(args[1], "on") != NULL) lightStatus |= 0b010;
  if (strstr(args[2], "on") != NULL) lightStatus |= 0b100;
  debug.info("Light status corrected, new status should be %s, %s, %s", args[0], args[1], args[2]);
}
