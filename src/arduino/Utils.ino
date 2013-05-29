void breatheLED(int pin, float strength=1) {
  // The value inside sin() controls speed, the multiplier at the end determines luminousity
  float val = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0 * strength;
  analogWrite(pin, val);
}

// Designed to be called inside loop()
void processQueuedPatterns() {
  if (!queuedPattern) return;
  debug.debug("Queued pattern found: `%lu`", queuedPattern);
  
  int signal = EtekcitySwitch::signalFromPattern(queuedPattern);
  queuedPattern = 0;
  debug.debug("Signal: %d (%s %s %s)", signal, BITWISE_STATUS(signal));
  lights.sendSignal(signal);
  
  // XOR toggles all bits in lightStatus where signal has a 1, leaves the rest alone
  lightStatus ^= signal;
  debug.info("Toggled lights with signal `%s %s %s`, new status `%s, %s, %s`", 
    BITWISE_STATUS(signal), BITWISE_STATUS(lightStatus));
  
  sendLightsStatus(); // Update the raspberry pi
}

void sendLightsStatus() {
  char lightStatusStr[18] = "lights_status ";
  itoa(lightStatus, lightStatusStr+strlen(lightStatusStr), 2); // last parameter = base (ie base 2 = binary)
  raspi.write(lightStatusStr);
}
