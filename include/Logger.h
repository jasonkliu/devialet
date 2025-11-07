#pragma once
#include <Arduino.h>

#define DEBUG_LOGGING 1  // Set to 0 for production

class Logger {
public:
  static void begin(unsigned long baudRate = 115200) {
    Serial.begin(baudRate);
  }
  
  // Always-on logging (errors, critical info)
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
  
#if DEBUG_LOGGING
  static void debugHttpGet(const String& url) {
    Serial.printf("→ GET %s\n", url.c_str());
  }
  
  static void debugHttpPost(const String& url, const String& body) {
    Serial.printf("→ POST %s\n  Body: %s\n", url.c_str(), body.c_str());
  }
  
  static void debugHttpResponse(int code, const String& body) {
    Serial.printf("← HTTP %d\n  Response: %s\n", code, body.c_str());
  }
  
  static void debugIR(const String& protocol, uint64_t value, int bits, bool repeat) {
    Serial.printf("◉ IR: %s 0x%llX (%d bits)%s\n", 
                  protocol.c_str(), (unsigned long long)value, bits, 
                  repeat ? " [REPEAT]" : "");
  }
#else
  static void debugHttpGet(const String&) {}
  static void debugHttpPost(const String&, const String&) {}
  static void debugHttpResponse(int, const String&) {}
  static void debugIR(const String&, uint64_t, int, bool) {}
#endif

  static void info(const String& msg) { Serial.printf("✓ %s\n", msg.c_str()); }
  static void error(const String& msg) { Serial.printf("✗ %s\n", msg.c_str()); }
  static void command(const String& cmd) { Serial.printf("→ %s\n", cmd.c_str()); }
};
