#pragma once
#include <WiFi.h>
#include <ESPmDNS.h>
#include "Logger.h"

class Connectivity {
private:
  const char* _ssid;
  const char* _pass;
  const char* _host;
  bool _mdns = false;
  unsigned long _lastConnectAttempt = 0;
  static constexpr unsigned long RECONNECT_INTERVAL_MS = 5000;

public:
  Connectivity(const char* ssid, const char* pass, const char* host = "m5-phantom")
    : _ssid(ssid), _pass(pass), _host(host) {}
  
  void connect() {
    if (WiFi.status() == WL_CONNECTED) return;
    if (millis() - _lastConnectAttempt < RECONNECT_INTERVAL_MS) return;
    
    _lastConnectAttempt = millis();
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pass);
    Logger::logf("WiFi connecting to %s\n", _ssid);
  }
  
  bool connectBlocking(uint16_t maxAttempts = 40) {
    if (WiFi.status() == WL_CONNECTED) return true;

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pass);
    Logger::logf("Connecting to %s\n", _ssid);

    for (uint16_t i = 0; i < maxAttempts && WiFi.status() != WL_CONNECTED; i++) {
      delay(250);
    }

    bool connected = WiFi.status() == WL_CONNECTED;
    if (connected) {
      Logger::logf("Connected: %s\n", WiFi.localIP().toString().c_str());
    } else {
      Logger::error("WiFi failed");
    }
    return connected;
  }
  
  bool isConnected() const { return WiFi.status() == WL_CONNECTED; }
  
  void ensureMDNS() {
    if (!isConnected() || _mdns) return;
    if (MDNS.begin(_host)) {
      _mdns = true;
      MDNS.addService("http", "tcp", 80);
      Logger::info("mDNS started");
    }
  }
  
  int queryMDNSService(const char* service, const char* protocol) {
    ensureMDNS();
    return MDNS.queryService(service, protocol);
  }
  
  String getMDNSHostname(int i) const { return MDNS.hostname(i); }
  IPAddress getMDNSIP(int i) const { return MDNS.IP(i); }
};
