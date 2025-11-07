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
  
  // Non-blocking WiFi connection with async behavior
  void connect() {
    if (WiFi.status() == WL_CONNECTED) return;

    unsigned long now = millis();
    if (now - _lastConnectAttempt < RECONNECT_INTERVAL_MS) {
      return; // Throttle reconnection attempts
    }
    
    _lastConnectAttempt = now;
    
    if (WiFi.status() == WL_IDLE_STATUS || WiFi.status() == WL_DISCONNECTED) {
      WiFi.mode(WIFI_STA);
      WiFi.begin(_ssid, _pass);
      Logger::logf("WiFi connecting to %s\n", _ssid);
    }
  }
  
  // Non-blocking connection check for setup phase only
  bool connectBlocking(uint16_t maxAttempts = 40) {
    if (WiFi.status() == WL_CONNECTED) return true;

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pass);
    Logger::logf("Connecting to %s\n", _ssid);

    for (uint16_t i = 0; i < maxAttempts && WiFi.status() != WL_CONNECTED; i++) {
      delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Logger::logf("Connected: %s\n", WiFi.localIP().toString().c_str());
      return true;
    }
    
    Logger::logln("WiFi failed");
    return false;
  }
  
  bool isConnected() { return WiFi.status() == WL_CONNECTED; }
  
  void ensureMDNS() {
    if (!isConnected() || _mdns) return;
    if (MDNS.begin(_host)) {
      _mdns = true;
      MDNS.addService("http", "tcp", 80);
      Logger::logln("mDNS started");
    }
  }
  
  int queryMDNSService(const char* service, const char* protocol) {
    ensureMDNS();
    return MDNS.queryService(service, protocol);
  }
  
  String getMDNSHostname(int i) { return MDNS.hostname(i); }
  IPAddress getMDNSIP(int i) { return MDNS.IP(i); }
};
