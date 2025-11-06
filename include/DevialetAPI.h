#pragma once
#include <Arduino.h>
#include <HTTPClient.h>
#include "Logger.h"

struct SpeakerState {
  String role;
  int volume = -1;
  bool enabled = true;
  bool isValid() const { return volume >= 0 && !role.isEmpty(); }
};

class DevialetAPI {
private:
  static constexpr int TIMEOUT_MS = 3000;
  
  int findInt(const String& json, const String& key) {
    int i = json.indexOf("\"" + key + "\":");
    if (i < 0) return -1;
    i += key.length() + 3;
    while (json[i] == ' ') i++;
    int end = i;
    while (isDigit(json[end])) end++;
    return (end > i) ? json.substring(i, end).toInt() : -1;
  }
  
  String findStr(const String& json, const String& key) {
    int i = json.indexOf("\"" + key + "\":\"");
    if (i < 0) return "";
    i += key.length() + 4;
    int end = json.indexOf('"', i);
    return (end > i) ? json.substring(i, end) : "";
  }

public:
  SpeakerState getState(const String& ip) {
    HTTPClient http;
    WiFiClient client;
    SpeakerState state;
    
    if (http.begin(client, "http://" + ip + "/opticaldirect/getall")) {
      http.setTimeout(TIMEOUT_MS);
      if (http.GET() == HTTP_CODE_OK) {
        String json = http.getString();
        state.volume = findInt(json, "volume");
        state.role = findStr(json, "role");
        state.enabled = json.indexOf("\"enabled\":true") >= 0;
        Logger::logf("%s: %s vol=%d\n", ip.c_str(), state.role.c_str(), state.volume);
      }
      http.end();
    }
    return state;
  }
  
  bool setVolume(const String& ip, int vol, const SpeakerState& state) {
    if (vol < 0 || vol > 100 || !state.isValid()) return false;
    
    HTTPClient http;
    WiFiClient client;
    
    if (http.begin(client, "http://" + ip + "/opticaldirect/volume")) {
      String body = "{\"enabled\":" + String(state.enabled ? "true" : "false") + 
                    ",\"role\":\"" + state.role + "\",\"volume\":" + vol + "}";
      http.addHeader("Content-Type", "application/json");
      bool ok = (http.POST(body) == HTTP_CODE_OK);
      http.end();
      if (ok) Logger::logf("%s: %d->%d\n", ip.c_str(), state.volume, vol);
      return ok;
    }
    return false;
  }
};
