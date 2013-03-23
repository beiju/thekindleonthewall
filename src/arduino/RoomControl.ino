#include <stdinout.h>
#include "Debug.h"
#include "DogRPC.h"
#include "Transciever.h"

#define LEDPIN 13
#define RFPIN 12
#define DOG_SIGNATURE "Dog"
#define BITRATE 9600

#define LIGHTS_SIG 0b0111000101010101
#define LIGHTS_TERMINATION 0b010

Debug debug;
DogRPC raspi;
Transciever lights;

void setup() {
  initializeSTDINOUT();
  pinMode(LEDPIN, OUTPUT);
  
  debug.begin(BITRATE);
  debug.enable(Debug::DEBUG);
  debug.enable(Debug::WARNING);
  debug.enable(Debug::INFO);
  
  raspi.begin(DOG_SIGNATURE, BITRATE, &debug);
  raspi.registerCommand("lights", lightsCmd);
  
  lights.begin(LIGHTS_SIG, LIGHTS_TERMINATION, RFPIN);
  
  debug.info("Setup complete");
}

void loop() {
  // Check for new commands
  raspi.refresh();
    
  // Update state of connected stuff
  
  // Read values from inputs
  
  delay(100); //!
  if (millis() % 2000 < 1000) {
    digitalWrite(LEDPIN, HIGH); 
  } else {
    digitalWrite(LEDPIN, LOW);
  }
    
}

void lightsCmd(String args[]) {
  // Pattern: 00AABBCC00 where A, B, C is the on/off state of lights 1, 2, and 3 (11 = on, 00 = off)
  int pattern = (args[0] == "on" ? 0b00001100 : 0b0);
  pattern += (args[1] == "on" ? 0b11000000 : 0b0);
  pattern += (args[2] == "on" ? 0b00110000 : 0b0);
  lights.send(pattern);
}
