#pragma once
#include <ArduinoJson.h>
#include "Logger.h"

class JsonHelper {
public:
  // Parse JSON string, return empty doc on failure
  static JsonDocument parse(const String& json) {
    JsonDocument doc;
    if (deserializeJson(doc, json)) {
      Logger::logHttpError("JSON parse failed");
    }
    return doc;
  }
  
  // Extract int with fallback, validates range
  static int getInt(JsonObject obj, const char* key, int defaultVal, int min = -1, int max = -1) {
    int val = obj[key] | defaultVal;
    if (min >= 0 && (val < min || val > max)) {
      Logger::logf("✗ %s out of range: %d (using %d)\n", key, val, defaultVal);
      return defaultVal;
    }
    return val;
  }
  
  // Extract string with fallback
  static String getString(JsonObject obj, const char* key, const char* defaultVal = "") {
    String val = obj[key] | defaultVal;
    if (val.isEmpty()) {
      Logger::logf("✗ Missing %s (using '%s')\n", key, defaultVal);
    }
    return val;
  }
  
  // Extract bool with fallback
  static bool getBool(JsonObject obj, const char* key, bool defaultVal) {
    return obj[key] | defaultVal;
  }
  
  // Build flat payload for POST (no "data" wrapper)
  static String buildDataPayload(const char* role, int volume, bool enabled) {
    JsonDocument doc;
    doc["enabled"] = enabled;
    doc["role"] = role;
    doc["volume"] = volume;
    
    String result;
    serializeJson(doc, result);
    return result;
  }
};
