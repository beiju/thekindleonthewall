#include "Arduino.h"
#include "Debug.h"
#include <stdarg.h>

// Default: errors only
void Debug::begin(int bitrate) {
  Serial.begin(bitrate);
  _enabled[INFO] = true;
  _enabled[WARNING] = false;
  _enabled[DEBUG] = true;
  this->debug("Debug constructor called");
  _enabled[DEBUG] = false;
  _enabled[ERROR] = true;
}

void Debug::printDebug(DebugType debugType, char* debugTypeStr, char* format, va_list args) {
  if (_enabled[debugType]) {
    // message: "TYPE formatted_debug_message"
    char* message = Utils::va_format(format, args);
    
    Serial.print(debugTypeStr);
    Serial.print(" ");
    Serial.println(message);
    Serial.flush();
  }
}

void Debug::info(char* messageStr, ...) {
  va_list args;
  va_start(args, messageStr);
  this->printDebug(INFO, "INFO", messageStr, args);
  va_end(args);
}

void Debug::warning(char* messageStr, ...) {
  va_list args;
  va_start(args, messageStr);
  this->printDebug(WARNING, "WARNING", messageStr, args);
  va_end(args);
}

void Debug::debug(char* messageStr, ...) {
  va_list args;
  va_start(args, messageStr);
  this->printDebug(DEBUG, "DEBUG", messageStr, args);
  va_end(args);
}

void Debug::error(char* messageStr, ...) {
  va_list args;
  va_start(args, messageStr);
  this->printDebug(ERROR, "ERROR", messageStr, args);
  va_end(args);
}

void Debug::enable(DebugType type) {
  _enabled[type] = true;
  this->info("Debug type %d enabled", (int) type);
}

void Debug::enable() {
  this->enable(INFO);
  this->enable(WARNING);
  this->enable(DEBUG);
  this->enable(ERROR);
}

void Debug::disable(DebugType type) {
  _enabled[type] = false;
  this->info("Debug type %d disabled", (int) type);
}

void Debug::disable() {
  this->disable(INFO);
  this->disable(WARNING);
  this->disable(DEBUG);
  this->disable(ERROR);
}

