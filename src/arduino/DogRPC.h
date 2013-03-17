#ifndef DogRPC_h
#define DogRPC_h

#include "Arduino.h"
#include "_utils.cpp"

#define CMD_MAX_LENGTH 8

class DogRPC {
  private:
    char* _signature;
    char* _message;
    Debug *_debug;
    
    struct CmdCallback {
      char* command;
      void (*function)(char*[]);
    };                                    // Data structure to hold Command/Handler function key-value pairs
    CmdCallback *_commands;   // Actual definition for command/handler array
    int _numCmds;
    
    void parseMessage();
    void evaluateCommand(char*, char*[]);
    char* nextMessageToken();
    int countSpaces(char*);
    
  public:
    DogRPC();
    void begin(char*, int);
    void begin(char*, int, Debug*);
    void refresh();
    void write(char*);
    unsigned long createChecksum(char*);
    void registerCommand(char*, void (*func)(char*[]));
    CmdCallback *findCommand(char*);
    boolean commandExists(char*);
    void runBareCommand(char*);
    void runCommand(char*, char*[]);
    
  
};

#endif
