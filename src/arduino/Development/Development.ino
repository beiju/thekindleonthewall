#include <stddef.h>
#include <stdinout.h>
#include <Debug.h>

Debug debug;
char* _message;
char* _signature = "Dog";

void setup() {
  Serial.begin(9600);
  initializeSTDINOUT();
  
  debug.begin(9600);
  debug.enable(Debug::DEBUG);
  debug.enable(Debug::WARNING);
  debug.enable(Debug::INFO);
  
  debug.info("Setup complete");
}

void loop() {
  _message = "Dog this is a test message dummyChecksum"
  ;
  parseMessage();
  delay(100000);
}

void parseMessage() {
  debug.debug("Parsing message `%s`", _message);
  

  char* signature = strtok(_message, " ");
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
  _message[*checksum - 1] = '\0'; // Replace the char before the message with a terminator to remove checksum from message
  debug.debug("Message after finding checksum: `%s`", _message);
  
  unsigned long computedChecksum = createChecksum(_message);
  debug.info("Computed checksum: `%lu` \t Computed on message: `%s`", computedChecksum, _message);
  if (computedChecksum != atol(checksum)) {
    _message = "";
    debug.error("Given checksum (%s) did not match expected checksum (%lu).", checksum, computedChecksum);
   return;
  }
  
  // At this point _message is the command and args
  char* command = strtok(NULL, " "); // NULL means start from where the last call to strtok() left off
  debug.info("Command: `%s`", command);
  
  // If the command was the last thing in the message, it's invalid (no checksum)
  if (strlen(_message) == 0) {
    debug.error("Messages must have 3 parts at minimum. Recieved message had 2.");
    return;
  }
/*  
  // At this point _message is only the args
  int argnum = DogRPC::countSpaces(_message);
  debug.debug("%d args detected.", argnum);
  String args[argnum+1]; // +1 is for NULL as the last element
  
  for (int i = 0; i < argnum; i++) {
    debug.debug("Getting arg %d from message `%s`.", i, _message);
    args[i] = nextMessageToken();
    debug.debug("arg %d: `%s`", i, args[i]);
  }
  args[argnum] = NULL;
  
  char checksum[strlen(_message)+1];
  strcpy(_message, checksum); // The checksum should be all that's left of the message
  debug.info("Checksum: `%s`", checksum);
  
  // Remove checksum from message body
  messageBody[strlen(messageBody) - strlen(checksum)] = '\0';
  unsigned long computedChecksum = createChecksum(messageBody);
  debug.info("Computed checksum: `%lu` \t Computed on message: `%s`", computedChecksum, messageBody);
  if (computedChecksum != atol(checksum)) {
    _message = "";
    debug.error("Given checksum (%s) did not match expected checksum (%lu).", checksum, computedChecksum);
   return;
  }
  
  runCommand(command, args);
  _message = "";*/
}

unsigned long createChecksum(char* msg) {
  unsigned long hash = 0;
  
  for (int i = 0; i < strlen(msg); i++) {
    hash = msg[i] + (hash << 6) + (hash << 16) - hash;
    debug.debug("Current char: `%d` \t Hash so far: %d", (int) msg[i], hash);
  }

//  // The above is an arduino-compatible version of:
//  int c;
//  while (c = *str++) {
//    hash = c + (hash << 6) + (hash << 16) - hash;
//  }
  
  return hash;
}
