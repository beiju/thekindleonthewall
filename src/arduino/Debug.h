#ifndef Debug_h
#define Debug_h

#include "Arduino.h"
#include "_utils.cpp"

class Debug {
  public:
    enum DebugType {
      INFO = 0,
      WARNING = 1,
      DEBUG = 2,
      ERROR = 3
    };
    
    void begin(int);
    static int freeRam();
    void info(char*, ...);
    void warning(char*, ...);
    void debug(char*, ...);
    void error(char*, ...);
    
    void enable();
    void disable();
    void enable(DebugType);
    void disable(DebugType);

  
  private:
    boolean _enabled[4];
    void printDebug(DebugType, char*, char*, va_list);
};

#endif
