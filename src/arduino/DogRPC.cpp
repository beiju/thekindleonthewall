#include "Arduino.h"
#include "Debug.h"
#include "DogRPC.h"

DogRPC::DogRPC() {}

void DogRPC::begin(String signature, int bitrate) {
  Debug debug;
  debug.begin(bitrate);
  debug.disable();
  begin(signature, bitrate, &debug);
}

void DogRPC::begin(String signature, int bitrate, Debug *debug) {
  _signature = signature;
  _debug = debug;
//  Serial.begin(bitrate);
  _numCmds = 0;
}

void DogRPC::registerCommand(char* cmd, void (*func)(String[])) {
  // Goal: Store commands and their functions in two arrays with corresponding indices.
  //   Store alphabetically by command to make lookups faster. 
  // Logic: Make a new array of size oldArraySize++. Iterate through the old array. If the 
  //   item's command is lower than the new command, copy it to the iteration index. If the commands are equal,
  //   throw an error. If the item's command is higher than the new command, and the new command has not been
  //   added, add the new command at the iteration index. Then add the old commands at the iteration index + 1.
  
  CmdCallback *newCmds = (CmdCallback*) malloc(sizeof(CmdCallback) * ++_numCmds); // Increment the number of commands and make an array of that size
  boolean cmdAdded = false;
  _debug->debug("Created new callback array with %d items", _numCmds);
  
  if (_numCmds == 1) { // this is the first command, so the for loop won't run at all
    _debug->debug("This command `%s` is the first command, adding manually", cmd);
    DogRPC::CmdCallback newCmd; // Add the command manually
    newCmd.command = cmd;
    newCmd.function = func;
    newCmds[0] = newCmd;
  }
  
  for (int i = 0; i < _numCmds-1; i++) { // _numCmds - 1 because it was just incremented
    _debug->debug("i = %d", i);
    if (String(_commands[i].command).compareTo(cmd) < 0) {
      newCmds[i] = _commands[i];
      _debug->debug("Copying command `%s` before new command `%s`", _commands[i].command, cmd);
    } else if (String(_commands[i].command).compareTo(cmd) == 0) {
      _debug->error("Command `%s` cannot be added because it already exists", cmd);
      return;
    } else {
      if (cmdAdded) {
        _debug->debug("Adding new command `%s` before command `%s`", cmd, _commands[i].command);
        DogRPC::CmdCallback newCmd;
        newCmd.command = cmd;
        newCmd.function = func;
        newCmds[i] = newCmd;
      }
      _debug->debug("Copying command `%s` after new command `%s`", _commands[i].command, cmd);
      newCmds[i+1] = _commands[i];
    }
  }
  
  _debug->debug("Copying newCmds into _commands");
  _commands = newCmds;
  _debug->info("Added command `%s`", cmd);
}

DogRPC::CmdCallback *DogRPC::findCommand(String cmd) {
  // Goal: Find the command efficiently by looking alphabetically
  // Logic: Set start and end to the start and end of the array. Then:
  //   1. Set index to the middle of the array (numerical average of start and end)
  //   2. If start == end, then the item was not found
  //   3. If the command of the index element is cmd, return it
  //   4. If the command of the index element is "less than" cmd, set end to index and repeat
  //   5. If the command of the index element is "greater than" cmd, set start to index and repeat
  
  int start = 0;
  int end = _numCmds;
  int index = _numCmds / 2; // intentional integer divison
  
  while (1) { // Break statement later on
    index = (start + end) / 2;
    if (start == end) {
      return NULL;
    } else if (String(_commands[index].command).compareTo(cmd) == 0) {
      return &_commands[index];
    } else if (String(_commands[index].command).compareTo(cmd) == -1) {
      end = index;
    } else {
      start = index;
    }
  }
}

boolean DogRPC::commandExists(String cmd) {
  return findCommand(cmd) == NULL;
}

void DogRPC::runBareCommand(String cmd) {
  String args[0] = {};
  runCommand(cmd, args);
}

void DogRPC::runCommand(String cmd, String args[]) {
  String argsStr = "";
  for (int i = 0; args[i] != NULL; i++) {
    argsStr += args[i];
    if (args[i+1] != NULL) argsStr += "`, `";
  }
  _debug->info("Running command: `%s` with args: `%s`.", cmd, argsStr);
  
   findCommand(cmd)->function(args);
}
  

void DogRPC::refresh() {
  char character;

  while(Serial.available()) {
    character = Serial.read();
    _message.concat(String(character));
    if (character == '\r') {
      DogRPC::parseMessage();
    }
    _debug->info("Message so far: %s", _message);
  }
  Serial.flush();
}

void DogRPC::parseMessage() {
  _message.trim(); // get rid of starting and ending whitespace
  
  String signature = DogRPC::nextMessageToken();
  _debug->info("Signature: `%s`", signature);
  if (signature != _signature) {
    _message = "";
    _debug->error("Given signature (%s) did not match expected signature (%s).", signature, _signature);
    return;
  }
  
  // If the signature was the last thing in the message, it's invalid (no command or checksum)
  if (_message == "") {
    //! Acknowledge invalid message recieved
    _debug->error("Messages must have 3 parts at minimum. Recieved message had 1.");
    return;
  }
  
  // At this point _message is the command, args, and checksum
  String messageBody = _message;
  
  String command = DogRPC::nextMessageToken();
  _debug->info("Command: `%s`", command);
  
  // If the command was the last thing in the message, it's invalid (no checksum)
  if (_message == "") {
    _debug->error("Messages must have 3 parts at minimum. Recieved message had 2.");
    return;
  }
  
  int argnum = DogRPC::countSpaces(_message);
  _debug->debug("%d args detected.", argnum);
  String args[argnum+1]; // +1 is for NULL as the last element
  
  for (int i = 0; i < argnum; i++) {
    _debug->debug("Getting arg %d from message `%s`.", i, _message);
    args[i] = nextMessageToken();
    _debug->debug("arg %d: `%s`", i, args[i]);
  }
  args[argnum] = NULL;
  
  String checksum = _message; // The checksum should be all that's left of the message
  _debug->info("Checksum: `%s`", checksum);
  
  // Remove checksum from message body
  messageBody = messageBody.substring(0, messageBody.length() - checksum.length() - 1);
  String computedChecksum = String(createChecksum(messageBody));
  _debug->info("Computed checksum: `%s` \t Computed on message: `%s`", computedChecksum, messageBody);
  if (computedChecksum != checksum) {
    _message = "";
    _debug->error("Given checksum (%S) did not match expected checksum (%s).", checksum, computedChecksum);
   return;
  }
  
  Serial.flush();
  runCommand(command, args);
  _message = "";
}

String DogRPC::nextMessageToken() {
  String token = _message.substring(0, _message.indexOf(" "));
  _message = _message.substring(token.length());
  _message.trim();
  _debug->info("Next token: `%s` \t New message: `%s`", token, _message);
  Serial.flush();
  return token;
}

int DogRPC::countSpaces(String str) {
  int count = 0;
  int nextIndex = str.indexOf(" ");
  while (nextIndex != -1) {
    _debug->debug("Count: %d \t nextIndex: %d \t strlen: %d", count, nextIndex, strlen);
    count++;
    nextIndex = str.indexOf(" ", nextIndex+1); // Start looking for the next right after the last
  }
  return count;
}

unsigned long DogRPC::createChecksum(String msg) {
  char str[msg.length()+1];
  msg.toCharArray(str, msg.length()+1);
  unsigned long hash = 0;
  
  for (int i = 0; i < msg.length(); i++) {
    hash = str[i] + (hash << 6) + (hash << 16) - hash;
    _debug->debug("Current char: `%d` \t Hash so far: %d", (int) str[i], hash);
  }

//  // The above is an arduino-compatible version of:
//  int c;
//  while (c = *str++) {
//    hash = c + (hash << 6) + (hash << 16) - hash;
//  }
  
  return hash;
}

void DogRPC::write(String message) {
  Serial.print( Utils::format("%s %s %s", _signature, message, createChecksum(message)) );
}
