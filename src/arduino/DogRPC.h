#ifndef DogRPC_h
#define DogRPC_h

#include "Arduino.h"
#include "_utils.cpp"

#define CMD_MAX_LENGTH 8

class DogRPC {
  private:
    String _signature;
    String _message;
    Debug *_debug;
    
    struct CmdCallback {
      char* command;
      void (*function)(String[]);
    };                                    // Data structure to hold Command/Handler function key-value pairs
    CmdCallback *_commands;   // Actual definition for command/handler array
    int _numCmds;
    
    void parseMessage();
    void evaluateCommand(char*, String[]);
    String nextMessageToken();
    int countSpaces(String);
    
  public:
    DogRPC();
    void begin(String, int);
    void begin(String, int, Debug*);
    void refresh();
    void write(String);
    unsigned long createChecksum(String);
    void registerCommand(char*, void (*func)(String[]));
    CmdCallback *findCommand(String);
    boolean commandExists(String);
    void runBareCommand(String);
    void runCommand(String, String[]);
    
  
};

#endif
