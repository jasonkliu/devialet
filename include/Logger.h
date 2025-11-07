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
  
  // HTTP debugging
  static void logHttpGet(const String& url) {
    Serial.printf("\n→ GET %s\n", url.c_str());
  }
  
  static void logHttpPost(const String& url, const String& body) {
    Serial.printf("\n→ POST %s\n  Body: %s\n", url.c_str(), body.c_str());
  }
  
  static void logHttpResponse(int code, const String& body) {
    Serial.printf("← HTTP %d\n  Response: %s\n", code, body.c_str());
  }
  
  static void logHttpError(const String& error) {
    Serial.printf("✗ HTTP Error: %s\n", error.c_str());
  }
  
  // IR debugging
  static void logIR(const String& protocol, uint32_t value, int bits, bool repeat) {
    Serial.printf("\n◉ IR RECV: proto=%s val=0x%X bits=%d repeat=%s\n", 
                  protocol.c_str(), value, bits, repeat ? "YES" : "NO");
  }
  
  static void logIRFiltered(const String& reason) {
    Serial.printf("  [FILTERED: %s]\n", reason.c_str());
  }
  
  static void logIRUnmapped() {
    Serial.printf("  [No command mapped]\n");
  }
  
  static void logIRCommand(const String& command) {
    Serial.printf("  → %s\n", command.c_str());
  }
  
  static void logIRNoise() {
    // Silent - reduce spam from noise
  }
};
