#include <MemoryFree.h>
#include <stdinout.h>
#include <vector.h>
#include <Debug.h>
#include <DogRPC.h>
#include <Transciever.h>

#define LEDPIN 13
#define RFPIN 12
#define DOG_SIGNATURE "Dog"
#define DOG_DEBUG_PREFIX "DOG"
#define BITRATE 9600

#define LIGHTS_SIG 0b0111000101010101
#define LIGHTS_TERMINATION 0b010

Debug debug;
DogRPC raspi;
Transciever lights;

/*************** Commands *****************/
// Note: commands have to be declared above setup() because Arduino can't generate headers for them

void lightsCmd(Vector<char*> args) {
  // Pattern: 00BBCCAA00 where A, B, C is the on/off state of lights 1, 2, and 3 (11 = on, 00 = off)
  int pattern = 0;
  pattern += (strstr(args[0], "on") != NULL ? 0b00001100 : 0b0);
  pattern += (strstr(args[1], "on") != NULL ? 0b11000000 : 0b0);
  pattern += (strstr(args[2], "on") != NULL ? 0b00110000 : 0b0);
  lights.send(pattern);
}

void testCmd(Vector<char*> args) {
  String argsStr = "";
  for (int i = 0; i < args.size(); i++) {
      argsStr += args[i];
      if (i < args.size()-1) argsStr += "`, `"; // don't add this the last time
  }
  char argsChars[argsStr.length()+1];
  argsStr.toCharArray(argsChars, argsStr.length()+1);
  
  debug.info("Recieved test command with args: `%s`", argsChars);
}

void setup() {
  Serial.begin(BITRATE);
  initializeSTDINOUT(); //! see if this needs to be here
  puts(" ");
  puts(" ");
  puts(" ");
  pinMode(LEDPIN, OUTPUT);
  
  debug.begin(Debug::DEBUG, BITRATE);
  
  raspi.begin(DOG_SIGNATURE, Debug::INFO, DOG_DEBUG_PREFIX);
  raspi.registerCommand("lights", lightsCmd);
  raspi.registerCommand("test", testCmd);
  
  lights.begin(LIGHTS_SIG, LIGHTS_TERMINATION, RFPIN);
  
  debug.info("RoomControl setup complete");
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
