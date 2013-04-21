void setup() {
  pinMode(DEBUG_LED, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  
  Serial.begin(BITRATE);
  initializeSTDINOUT(); //! see if this needs to be here
  puts(" ");
  puts(" ");
  puts(" ");
  debug.begin(Debug::DEBUG, BITRATE);
  
  raspi.begin(DOG_SIGNATURE, Debug::INFO, DOG_DEBUG_PREFIX);
  raspi.registerCommand("lights", lightsCmd);
  raspi.registerCommand("test", testCmd);
  raspi.registerCommand("send_status", statusRequestCmd);
  
  RemoteReceiver::init(RF_RECIEVER_INTERRUPT, LIGHTS_SIGNAL_REPETITIONS, rfSignalRecievedCmd);
  pinMode(LIGHT_BTN_LED, OUTPUT);
  lightStatus = 0b011; // Assume 1 and 2 are on, 3 is off
  
  debug.info("RoomControl setup complete");
  Serial.flush();
}

void loop() {
  // Check for new commands
  raspi.refresh();
  processQueuedPatterns();
    
  // Update state of connected stuff
  // Light button LED
  if ((lightStatus & 0b011) == 0b000) { // if lights are off (the & 0b110 makes light 3 not matter)
    digitalWrite(LIGHT_BTN_LED, HIGH);
  } else {
    digitalWrite(LIGHT_BTN_LED, LOW);
  }
  if (millis() % 2000 == 0) debug.debug("Lights: %s %s %s, previousLightTime: %lu, previousLightSignal: %lu", 
                              BIT_ON(lightStatus, 0b001), BIT_ON(lightStatus, 0b010), BIT_ON(lightStatus, 0b100), 
                              previousLightTime, previousLightSignal);    
  
  // Read values from inputs
  lightsBtn.update();
  
  // IT'S ALIIIVE
  breatheLED(STATUS_LED_PIN);
}
