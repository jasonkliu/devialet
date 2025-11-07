#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include "Logger.h"
#include "JsonHelper.h"

struct SpeakerState {
  String role;
  int volume = -1;
  bool enabled = true;
  bool isValid() const { return volume >= 0 && !role.isEmpty(); }
};

class DevialetAPI {
private:
  static constexpr int TIMEOUT_MS = 300;

public:
  SpeakerState getState(const String& ip) {
    HTTPClient http;
    WiFiClient client;
    SpeakerState state;
    
    String url = "http://" + ip + "/opticaldirect/getall";
    if (!http.begin(client, url)) return state;
    
    Logger::debugHttpGet(url);
    http.setTimeout(TIMEOUT_MS);
    int code = http.GET();
    String payload = http.getString();
    http.end();
    
    Logger::debugHttpResponse(code, payload);
    if (code != HTTP_CODE_OK) return state;
    
    JsonDocument doc = JsonHelper::parse(payload);
    JsonObject data = doc["data"];
    if (data.isNull()) return state;
    
    state.volume = JsonHelper::getInt(data, "volume", 0, 0, 100);
    state.role = JsonHelper::getString(data, "role", "Unknown");
    state.enabled = JsonHelper::getBool(data, "enabled", true);
    
    return state;
  }
  
  bool setVolume(const String& ip, int vol, const SpeakerState& state) {
    if (vol < 0 || vol > 100 || !state.isValid()) return false;
    
    HTTPClient http;
    WiFiClient client;
    
    String url = "http://" + ip + "/opticaldirect/volume";
    if (!http.begin(client, url)) return false;
    
    String body = JsonHelper::buildFlatPayload(state.role.c_str(), vol, state.enabled);
    Logger::debugHttpPost(url, body);
    
    http.setTimeout(TIMEOUT_MS);
    http.addHeader("Content-Type", "application/json");
    
    int code = http.POST(body);
    String response = http.getString();
    http.end();
    
    Logger::debugHttpResponse(code, response);
    return (code == HTTP_CODE_OK);
  }
};
