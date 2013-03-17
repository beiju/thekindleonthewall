#ifndef Debug_h
#define Debug_h

#include "Arduino.h"
#include "_utils.cpp"

#define s(arg) Utils::str_to_char(arg)

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
    void info(String, ...);
    void warning(String, ...);
    void debug(String, ...);
    void error(String, ...);
    
    void enable();
    void disable();
    void enable(DebugType);
    void disable(DebugType);

  
  private:
    boolean _enabled[4];
    void printDebug(DebugType, String, String, va_list);
    static String va_format(const prog_char*, va_list);
    static String format(const prog_char*, ...);
};

#endif
