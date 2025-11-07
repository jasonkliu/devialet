#pragma once

#define DECODE_SAMSUNG 1

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "Logger.h"

enum class IRCommand { None, VolumeUp, VolumeDown, Mute };

constexpr unsigned long BUTTON_REPEAT_INTERVAL_MS = 500;

namespace RawIR {
  static constexpr uint64_t VOL_UP = 0x0;
  static constexpr uint64_t VOL_DOWN = 0x0;
  static constexpr uint64_t MUTE = 0x0;
}

class IRReceiver {
private:
  // Samsung TV remote codes
  static constexpr uint32_t IR_CODE_SAMSUNG_VOL_UP = 0xE0E0E01F;
  static constexpr uint32_t IR_CODE_SAMSUNG_VOL_DOWN = 0xE0E0D02F;
  static constexpr uint32_t IR_CODE_SAMSUNG_MUTE = 0xE0E0F00F;
  
  IRrecv _recv;
  decode_results _res;
  IRCommand _last = IRCommand::None;
  unsigned long _lastCommandTime = 0;
  
  IRCommand decode(uint64_t val) const {
    if (val == IR_CODE_SAMSUNG_VOL_UP || val == RawIR::VOL_UP) return IRCommand::VolumeUp;
    if (val == IR_CODE_SAMSUNG_VOL_DOWN || val == RawIR::VOL_DOWN) return IRCommand::VolumeDown;
    if (val == IR_CODE_SAMSUNG_MUTE || val == RawIR::MUTE) return IRCommand::Mute;
    return IRCommand::None;
  }

public:
  IRReceiver(uint8_t pin) : _recv(pin) {}
  
  void begin() { _recv.enableIRIn(); }
  
  IRCommand check() {
    if (!_recv.decode(&_res)) return IRCommand::None;
    
    if (_res.value == 0xFFFFFFFF || _res.value == 0 || _res.bits < 8) {
      _recv.resume();
      return IRCommand::None;
    }
    
    Logger::debugIR(typeToString(_res.decode_type), _res.value, _res.bits, _res.repeat);
    
    IRCommand cmd = decode(_res.value);
    if (cmd != IRCommand::None) {
      unsigned long now = millis();
      if (cmd == _last && now - _lastCommandTime < BUTTON_REPEAT_INTERVAL_MS) {
        _recv.resume();
        return IRCommand::None;
      }
      Logger::command(toString(cmd));
      _last = cmd;
      _lastCommandTime = now;
    }
    
    _recv.resume();
    return cmd;
  }
  
  static String toString(IRCommand cmd) {
    switch (cmd) {
      case IRCommand::VolumeUp: return "VOL UP";
      case IRCommand::VolumeDown: return "VOL DOWN";
      case IRCommand::Mute: return "MUTE";
      default: return "UNKNOWN";
    }
  }
};
