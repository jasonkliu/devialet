#pragma once
#include <Arduino.h>
#include <vector>
#include <algorithm>
#include "DevialetAPI.h"
#include "IRReceiver.h"
#include "Connectivity.h"
#include "Display.h"

class Speaker {
private:
  String _ip;
  SpeakerState _state;
  DevialetAPI* _api;
  
  int calcNewVolume(IRCommand cmd) const {
    if (!_state.isValid()) return -1;
    switch (cmd) {
      case IRCommand::VolumeUp:   return min(_state.volume + 5, 100);
      case IRCommand::VolumeDown: return max(_state.volume - 5, 0);
      case IRCommand::Mute:       return 0;
      default: return -1;
    }
  }
  
  char getRoleLetter() const {
    if (_state.role.indexOf("Left") >= 0) return 'L';
    if (_state.role.indexOf("Right") >= 0) return 'R';
    if (_state.role.indexOf("Mono") >= 0) return 'M';
    return '?';
  }

public:
  Speaker(const String& ip, DevialetAPI* api) : _ip(ip), _api(api) {}
  
  const String& ip() const { return _ip; }
  const String& role() const { return _state.role; }
  int volume() const { return _state.volume; }
  
  void updateVolume() { _state = _api->getState(_ip); }
  
  int predictVolume(IRCommand cmd) {
    if (!_state.isValid()) updateVolume();
    return calcNewVolume(cmd);
  }
  
  bool execute(IRCommand cmd) {
    int newVol = predictVolume(cmd);
    if (newVol < 0 || newVol == _state.volume) return newVol >= 0;
    
    if (_api->setVolume(_ip, newVol, _state)) {
      _state.volume = newVol;
      return true;
    }
    return false;
  }
  
  SpeakerDisplayInfo getDisplayInfo() const {
    return { getRoleLetter(), _state.isValid() ? _state.volume : 0, _ip };
  }
};

class SpeakerManager {
private:
  std::vector<Speaker> _speakers;
  DevialetAPI _api;
  unsigned long _lastScan = 0;
  
  std::vector<String> scanNetwork(Connectivity& wifi) {
    std::vector<String> ips;
    int n = wifi.queryMDNSService("http", "tcp");
    for (int i = 0; i < n; i++) {
      String host = wifi.getMDNSHostname(i);
      host.toLowerCase();
      if (host.indexOf("phantom") >= 0) {
        String ip = wifi.getMDNSIP(i).toString();
        if (!ip.isEmpty() && std::find(ips.begin(), ips.end(), ip) == ips.end()) {
          ips.push_back(ip);
        }
      }
    }
    return ips;
  }

public:
  void discover(Connectivity& wifi, bool force = false) {
    if (!force && (millis() - _lastScan) < 60000) return;
    
    auto ips = scanNetwork(wifi);
    bool changed = force || _speakers.size() != ips.size();
    
    if (!changed) {
      for (auto& sp : _speakers) {
        if (std::find(ips.begin(), ips.end(), sp.ip()) == ips.end()) {
          changed = true;
          break;
        }
      }
    }
    
    if (changed) {
      _speakers.clear();
      for (const auto& ip : ips) _speakers.emplace_back(ip, &_api);
      Logger::logf("Found %u speaker(s)\n", _speakers.size());
    }
    _lastScan = millis();
  }
  
  void refreshVolumes() {
    for (auto& sp : _speakers) {
      sp.updateVolume();
      delay(50);
    }
  }
  
  int executeAll(IRCommand cmd) {
    int ok = 0;
    for (auto& sp : _speakers) {
      if (sp.execute(cmd)) ok++;
      delay(50);
    }
    return ok;
  }
  
  int predictVolumeMin(IRCommand cmd) {
    if (_speakers.empty()) return -1;
    int minVol = 101;
    for (auto& sp : _speakers) {
      int v = sp.predictVolume(cmd);
      if (v >= 0 && v < minVol) minVol = v;
    }
    return minVol <= 100 ? minVol : -1;
  }
  
  size_t count() const { return _speakers.size(); }
  bool needsScan() const { return (millis() - _lastScan) >= 60000; }
  
  std::vector<SpeakerDisplayInfo> getInfo() const {
    std::vector<Speaker> sorted = _speakers;
    std::sort(sorted.begin(), sorted.end(), [](const Speaker& a, const Speaker& b) {
      // Sort order: FrontLeft, FrontRight, Mono, others
      auto getRank = [](const String& role) {
        if (role.indexOf("Left") >= 0) return 1;
        if (role.indexOf("Right") >= 0) return 2;
        if (role.indexOf("Mono") >= 0) return 3;
        return 4;
      };
      return getRank(a.role()) < getRank(b.role());
    });
    
    std::vector<SpeakerDisplayInfo> info;
    for (const auto& sp : sorted) info.push_back(sp.getDisplayInfo());
    return info;
  }
};
