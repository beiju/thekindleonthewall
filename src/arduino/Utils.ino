void breatheLED(int pin) {
  // The value inside sin() controls speed, the multiplier at the end determines luminousity
  float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
  analogWrite(pin, val);
}

/* suppressDebug is needed because this function is sometimes called from inside interrupts */
int lightsFromPattern(unsigned long pattern, boolean suppressDebug = false) {
  int code;
  switch (pattern) {
    case LIGHT_1_CODE:   code = 0b001; break;
    case LIGHT_2_CODE:   code = 0b010; break;
    case LIGHT_3_CODE:   code = 0b100; break;
    case LIGHT_12_CODE:  code = 0b011; break;
    case LIGHT_13_CODE:  code = 0b101; break;
    case LIGHT_23_CODE:  code = 0b110; break;
    case LIGHT_123_CODE: code = 0b111; break;
    default: code = 0; break;
  }
  if (!suppressDebug) debug.debug("Generated code `%d` from pattern `%lu`", code, pattern);
  return code;
}

/* suppressDebug is needed because this function is sometimes called from inside interrupts */
unsigned long patternFromLights(int code, boolean suppressDebug = false) {
  unsigned long pattern;
  switch (code) {
    case 0b001: pattern = LIGHT_1_CODE; break;
    case 0b010: pattern = LIGHT_2_CODE; break;
    case 0b100: pattern = LIGHT_3_CODE; break;
    case 0b011: pattern = LIGHT_12_CODE; break;
    case 0b101: pattern = LIGHT_13_CODE; break;
    case 0b110: pattern = LIGHT_23_CODE; break;
    case 0b111: pattern = LIGHT_123_CODE; break;
    default: pattern = 0; break;
  }
  if (!suppressDebug) debug.debug("Generated pattern `%lu` from code `%d`", pattern, code);
  return pattern;
}

// Modified from Retransmitter example in RemoteReciever package
void sendLightsPattern(unsigned long pattern) {
  debug.info("Sending pattern `%lu` to lights", pattern);
  //Disable the receiver; otherwise it might pick up the retransmit as well.
  RemoteReceiver::disable();
  
  unsigned long code;
  
  //Copy the received code. 
  code = pattern & 0xFFFFF; //truncate to 20 bits for show; receivedCode is never more than 20 bits..
  
  //Add the period duration to the code. Range: [0..511] (9 bit)
  code |= (unsigned long) LIGHTS_PERIOD_DURATION << 23;
  
  //Add the number of repeats to the code. Range: [0..7] (3 bit). The actual number of repeats will be 2^(repeats), 
  //in this case 8
  code |= 3L << 20;

  //Retransmit the signal on pin 11. Note: no object was created!
  RemoteSwitch::sendTelegram(code, RF_TRANSMITTER_PIN);
  
  RemoteReceiver::enable();
}


void sendLightsCode(int code) {
  debug.info("Sending code `%lu` to lights", code);
  unsigned long pattern = patternFromLights(code);
  debug.debug("Pattern: %lu", pattern);
  sendLightsPattern(pattern);
}  
