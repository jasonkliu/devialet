#pragma once
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

enum class IRCommand { None, VolumeUp, VolumeDown, Mute };

class IRReceiver {
private:
  IRrecv _recv;
  decode_results _res;
  bool _active = false;
  unsigned long _lastSeen = 0;
  IRCommand _last = IRCommand::None;
  
  static constexpr uint64_t LG_VOL_UP = 0x20DF40BFULL;
  static constexpr uint64_t LG_VOL_DOWN = 0x20DFC03FULL;
  static constexpr uint64_t LG_MUTE = 0x20DF906FULL;

public:
  IRReceiver(uint8_t pin) : _recv(pin) {}
  
  void begin() { _recv.enableIRIn(); _active = true; }
  bool isActive() const { return _active; }
  bool hadRecentActivity(unsigned long ms = 5000) const {
    return _lastSeen && (millis() - _lastSeen) < ms;
  }
  
  IRCommand checkForCommand(String* debug = nullptr) {
    if (!_recv.decode(&_res)) return IRCommand::None;
    
    _lastSeen = millis();
    if (debug) *debug = "0x" + String((unsigned long long)_res.value, HEX);
    
    IRCommand cmd = IRCommand::None;
    if (_res.value == LG_VOL_UP) cmd = IRCommand::VolumeUp;
    else if (_res.value == LG_VOL_DOWN) cmd = IRCommand::VolumeDown;
    else if (_res.value == LG_MUTE) cmd = IRCommand::Mute;
    else if (_res.repeat && _last != IRCommand::None) cmd = _last;
    
    _recv.resume();
    if (cmd != IRCommand::None) _last = cmd;
    return cmd;
  }
  
  static String commandToString(IRCommand cmd) {
    switch (cmd) {
      case IRCommand::VolumeUp: return "VOL UP";
      case IRCommand::VolumeDown: return "VOL DOWN";
      case IRCommand::Mute: return "MUTE";
      default: return "UNKNOWN";
    }
  }
};
