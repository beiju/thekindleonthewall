void setup() {
  pinMode(DEBUG_LED, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  
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
  raspi.registerCommand("calinfo", calInfoCmd);
  
  RemoteReceiver::init(RF_RECIEVER_INTERRUPT, LIGHTS_SIGNAL_REPETITIONS, rfSignalRecievedCmd);
  pinMode(LIGHT_BTN_LED, OUTPUT);
  lightStatus = ENABLED_LIGHTS; // Assume all enabled lights are on
  
  debug.info("RoomControl setup complete");
  Serial.flush();
}

void loop() {
  // Check for new commands
  raspi.refresh();
  processQueuedPatterns();
    
  // Update state of connected stuff
  // Light button LED
  if ((lightStatus & ENABLED_LIGHTS) == 0b000) { // if lights are off (the & ENABLED_LIGHTS ignores disabled)
    digitalWrite(LIGHT_BTN_LED, HIGH);
  } else {
    digitalWrite(LIGHT_BTN_LED, LOW);
  }
  unsigned int will_nextEvent_diff = nextEventTime_Will - millis();
  if (millis() % 2000 == 0) debug.debug("Lights: %s %s %s, previousLightTime: %lu, previousLightSignal: %lu, " 
                              "nextEvtDiff: %lu message: %s", BITWISE_STATUS(lightStatus), 
                              previousLightTime, previousLightSignal, will_nextEvent_diff, raspi.message());    
  
  // Read values from inputs
  lightsBtn.update();
  
  // Update calendar stuff
//  unsigned int will_nextEvent_diff = nextEventTime_Will - millis();
  if (debugFlag == true) {
    statusColors[0] = 0;
    statusColors[1] = 1;
    statusColors[2] = 0;
  } else if (will_nextEvent_diff < millis_15min  && nextEventTime_Will != 0) {
    unsigned int d = .5 * millis_15min / (1+will_nextEvent_diff); // add one to prevent divide by 0
    statusColors[0] = .5 + d; // Red starts at .5 and gets larger
    statusColors[1] = .5 - d; // Green starts at .5 and gets larger
    statusColors[2] = .5 - d; // Blue starts at .5 and gets larger
  } else {
    statusColors[0] = .5;
    statusColors[1] = .5;
    statusColors[2] = .5;
  }
  
  // IT'S ALIIIVE
  breatheLED(RED_LED_PIN, statusColors[0]);
  breatheLED(GREEN_LED_PIN, statusColors[1]);
  breatheLED(BLUE_LED_PIN, statusColors[2]);
}
