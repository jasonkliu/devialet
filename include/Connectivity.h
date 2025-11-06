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

public:
  Connectivity(const char* ssid, const char* pass, const char* host = "m5-phantom")
    : _ssid(ssid), _pass(pass), _host(host) {}
  
  void connect() {
    if (WiFi.status() == WL_CONNECTED) return;

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _pass);
    Logger::logf("Connecting to %s\n", _ssid);

    for (uint8_t i = 0; i < 40 && WiFi.status() != WL_CONNECTED; i++) {
      delay(250);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Logger::logf("Connected: %s\n", WiFi.localIP().toString().c_str());
    } else {
      Logger::logln("WiFi failed");
    }
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
