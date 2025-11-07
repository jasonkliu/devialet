#pragma once
#include <vector>
#include "DevialetAPI.h"
#include "IRReceiver.h"
#include "Connectivity.h"
#include "Display.h"
#include "Logger.h"

class Speaker {
private:
  String _ip;
  SpeakerState _state;
  int _optimisticVolume = -1;
  int _preMuteVolume = -1;
  
  int calcVolume(IRCommand cmd, int currentVol) const {
    switch (cmd) {
      case IRCommand::VolumeUp: return min(currentVol + 5, 100);
      case IRCommand::VolumeDown: return max(currentVol - 5, 0);
      case IRCommand::Mute: 
        return (currentVol == 0 && _preMuteVolume > 0) ? _preMuteVolume : 0;
      default: return -1;
    }
  }
  
  char roleLetter() const {
    if (_state.role.indexOf("Left") >= 0) return 'L';
    if (_state.role.indexOf("Right") >= 0) return 'R';
    if (_state.role.indexOf("Mono") >= 0) return 'M';
    return '?';
  }

public:
  Speaker(const String& ip) : _ip(ip) {}
  
  const String& ip() const { return _ip; }
  bool isValid() const { return _state.isValid(); }
  
  void refresh(DevialetAPI& api) { 
    _state = api.getState(_ip);
    _optimisticVolume = _state.volume;
  }
  
  bool execute(DevialetAPI& api, IRCommand cmd) {
    refresh(api);
    if (!_state.isValid()) return false;
    
    int targetVol = calcVolume(cmd, _state.volume);
    if (targetVol < 0) return false;
    if (targetVol == _state.volume) return true;
    
    // Save volume before muting
    if (cmd == IRCommand::Mute && _state.volume > 0) {
      _preMuteVolume = _state.volume;
    }
    
    _optimisticVolume = targetVol;
    
    bool ok = api.setVolume(_ip, targetVol, _state);
    if (ok) {
      refresh(api);  // Verify actual state after POST
    }
    
    return true;
  }
  
  SpeakerDisplayInfo info() const {
    return { roleLetter(), _optimisticVolume >= 0 ? _optimisticVolume : 0, _ip };
  }
};

class SpeakerManager {
private:
  std::vector<Speaker> _speakers;
  DevialetAPI _api;

public:
  void discover(Connectivity& wifi) {
    _speakers.clear();
    int n = wifi.queryMDNSService("http", "tcp");
    for (int i = 0; i < n; i++) {
      String host = wifi.getMDNSHostname(i);
      host.toLowerCase();
      if (host.indexOf("phantom") >= 0) {
        _speakers.emplace_back(wifi.getMDNSIP(i).toString());
      }
    }
    Logger::logf("Found %u speaker(s)\n", _speakers.size());
  }
  
  void refresh() {
    for (auto& sp : _speakers) {
      sp.refresh(_api);
      yield();
    }
  }
  
  int executeAll(IRCommand cmd) {
    int ok = 0;
    for (auto& sp : _speakers) {
      if (sp.execute(_api, cmd)) ok++;
      yield();
    }
    return ok;
  }
  
  size_t count() const { return _speakers.size(); }
  
  bool hasValidSpeakers() const {
    for (const auto& sp : _speakers) {
      if (sp.isValid()) return true;
    }
    return false;
  }
  
  std::vector<SpeakerDisplayInfo> info() const {
    std::vector<SpeakerDisplayInfo> result;
    result.reserve(_speakers.size());
    
    for (const auto& sp : _speakers) {
      result.push_back(sp.info());
    }
    
    std::sort(result.begin(), result.end(), [](const SpeakerDisplayInfo& a, const SpeakerDisplayInfo& b) {
      if (a.role == 'L') return true;
      if (b.role == 'L') return false;
      if (a.role == 'R') return true;
      if (b.role == 'R') return false;
      return a.role < b.role;
    });
    
    return result;
  }
};
