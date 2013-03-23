#include <stddef.h>
#include <stdinout.h>
#include <vector.h>
#include <Debug.h>

Debug debug;
char* _message;
char* _signature = "Dog";

void setup() {
  Serial.begin(9600);
  initializeSTDINOUT();
  
  debug.begin(9600);
//  debug.enable(Debug::DEBUG);
  debug.enable(Debug::WARNING, Debug::INFO);
  
  debug.info("Setup complete");
}

void loop() {
  _message = "Dog this is a test message 4262768940";
  parseMessage();
  while (1) delay(100000);
}

// Arduino can't generate headers for a function with the <> thing, so it has to be declared before it's used
void runCommand(char* cmd, Vector<char*> args) {
  if (debug.enabled(Debug::INFO)) { // don't do all this formatting unless it's going to be used
    String argsStr = "";
    for (int i = 0; i < args.size(); i++) {
      argsStr += args[i];
      if (i < args.size()-1) argsStr += "`, `"; // don't add this the last time
    }
    char argsChars[argsStr.length()+1];
    argsStr.toCharArray(argsChars, argsStr.length()+1);
    debug.info("Running command: `%s` with args: `%s`.", cmd, argsChars);
  }
  
  // findCommand(cmd)->function(args);
}

void parseMessage() {
  debug.debug("Parsing message `%s`", _message);
  
  _message = strtok(_message, " "); // message now points to the beginning of signature (necessary to handle extra spaces)
  char* signature = _message;
  _message += strlen(signature)+1; // Advance the pointer to the char after signature's terminator
  debug.info("Signature: `%s`", signature);
  if (strcmp(signature, _signature) != 0) {
    _message = "";
    debug.error("Given signature (%s) did not match expected signature (%s).", signature, _signature);
    return;
  }
  
  // If the signature was the last thing in the message, it's invalid (no command or checksum)
  if (strlen(_message) == 0) {
    //! Acknowledge invalid message recieved
    debug.error("Messages must have 3 parts at minimum. Recieved message had 1.");
    return;
  }
  
  // At this point _message is the command, args, and checksum
  debug.debug("Message including checksum: `%s`", _message);
  char* checksum = strrchr(_message, ' ') + 1; // Find last space in the message and add one to it to get checksum
  debug.debug("Checksum: `%s`", checksum);
  *(checksum-1) = '\0';
  _message[*checksum - 1] = '\0'; // Replace the char before the checksum with a terminator to remove checksum from message
  debug.debug("Message after finding checksum: `%s`", _message);
  
  unsigned long computedChecksum = createChecksum(_message);
  debug.debug("Computed checksum: `%lu` \t Computed on message: `%s`", computedChecksum, _message);
  if (computedChecksum != atol(checksum)) {
    _message = "";
    debug.error("Given checksum (%s) did not match expected checksum (%lu).", checksum, computedChecksum);
   return;
  } else {
    debug.info("Checksum is valid");
  }
  
  // If the checksum was the last thing in the message, it's invalid (no command)
  if (strlen(_message) == 0) {
    debug.error("Messages must have 3 parts at minimum. Recieved message had 2.");
    return;
  }
  
  // At this point _message is the command and args
  char* command = _message;
  _message = strtok(NULL, " "); // NULL means start from where the last call to strtok() left off
  debug.info("Command: `%s`", command);
  
  // At this point _message is only the args
  Vector<char*> args;
  char *currArg = strtok(NULL, " ");
  while (currArg != NULL) {
    args.push_back(currArg);
    debug.debug("Adding arg `%s` to list of args", currArg);
    currArg = strtok(NULL, " ");
  }
  
  runCommand(command, args);
  _message = "";
}

unsigned long createChecksum(char* msg) {
  unsigned long hash = 0;
  
  for (int i = 0; i < strlen(msg); i++) {
    hash = msg[i] + (hash << 6) + (hash << 16) - hash;
//    debug.debug("Current char: `%d` \t Hash so far: %lu", (int) msg[i], hash);
  }

//  // The above is an arduino-compatible version of:
//  int c;
//  while (c = *str++) {
//    hash = c + (hash << 6) + (hash << 16) - hash;
//  }
  
  return hash;
}

