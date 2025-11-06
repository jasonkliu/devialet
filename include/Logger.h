#pragma once
#include <Arduino.h>

class Logger {
public:
  static void begin(unsigned long baudRate = 115200) {
    Serial.begin(baudRate);
  }
  
  template<typename... Args>
  static void log(Args... args) {
    Serial.print(args...);
  }
  
  template<typename... Args>
  static void logln(Args... args) {
    Serial.println(args...);
  }
  
  template<typename... Args>
  static void logf(const char* format, Args... args) {
    Serial.printf(format, args...);
  }
};
