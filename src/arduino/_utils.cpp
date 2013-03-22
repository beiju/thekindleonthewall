#ifndef Utils //! TODO: More unique name
#define Utils

#import "Arduino.h"

#define FORMAT_CHARS 256


namespace Utils {
  // Note: These functions cannot reference functions defined below them, so definition order matters
  
  char* str_to_char(String str) {
    char chars[str.length() + 1];
    str.toCharArray(chars, str.length() + 1);
    return chars;
  }
  
  char* va_format(String str, va_list args) {   
    char tmp[FORMAT_CHARS]; // resulting string limited to FORMAT_CHARS chars
    char* fmt = str_to_char(str);
    
    vsnprintf(tmp, FORMAT_CHARS, fmt, args);
    
    return tmp;
  }
  
  char* format(PGM_P format, ...) {
    va_list args;
    va_start(args,format);
    
    char* result = va_format(format, args);
    
    va_end(args);
    return result;
  }

}

#endif
