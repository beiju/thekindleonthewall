#ifndef Utils //! TODO: More unique name
#define Utils

#import "Arduino.h"

#define FORMAT_CHARS 256

namespace Utils {
  char* va_format(String str, va_list args) {   
    char fmt[FORMAT_CHARS]; // format string limited to FORMAT_CHARS chars
    char tmp[FORMAT_CHARS]; // resulting string limited to FORMAT_CHARS chars
    str.toCharArray(fmt, FORMAT_CHARS);
    
    vsnprintf(tmp, FORMAT_CHARS, fmt, args);
    
    return tmp;
  }

  String _va_format(PGM_P format, va_list args) {
    // program memory version of printf - copy of format string and result share a buffer
    // so as to avoid too much memory use
    char formatString[128], *ptr;
    strncpy_P( formatString, format, sizeof(formatString) ); // copy in from program mem
    // null terminate - leave last char since we might need it in worst case for result's \0
    formatString[ sizeof(formatString)-2 ]='\0'; 
    ptr=&formatString[ strlen(formatString)+1 ]; // our result buffer...
    vsnprintf(ptr, sizeof(formatString)-1-strlen(formatString), formatString, args );
    formatString[ sizeof(formatString)-1 ]='\0'; 
    return String(ptr);
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
