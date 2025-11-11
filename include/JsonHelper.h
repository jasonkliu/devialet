#pragma once
#include <ArduinoJson.h>
#include "Logger.h"

class JsonHelper {
public:
  static JsonDocument parse(const String& json) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
      Logger::logf("✗ JSON parse failed: %s\n", error.c_str());
    }
    return doc;
  }
  
  static int getInt(JsonObject obj, const char* key, int defaultVal, int min = -1, int max = -1) {
    int val = obj[key] | defaultVal;
    if (min >= 0 && (val < min || val > max)) {
      val = defaultVal;
    }
    return val;
  }
  
  static String getString(JsonObject obj, const char* key, const char* defaultVal = "") {
    return obj[key] | defaultVal;
  }
  
  static bool getBool(JsonObject obj, const char* key, bool defaultVal) {
    return obj[key] | defaultVal;
  }
  
  static String buildFlatPayload(const char* role, int volume, bool enabled) {
    JsonDocument doc;
    doc["enabled"] = enabled;
    doc["role"] = role;
    doc["volume"] = volume;
    
    String result;
    serializeJson(doc, result);
    return result;
  }
};
