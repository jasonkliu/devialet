#pragma once

#define DECODE_LG 1
#define DECODE_NEC 1
#define DECODE_SAMSUNG 1

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include "Logger.h"

enum class IRCommand { None, VolumeUp, VolumeDown, Mute };

// RAW IR codes for your specific remote (50-bit UNKNOWN protocol)
// Add your codes here after identifying them
namespace RawIR {
  static constexpr uint64_t VOL_UP = 0x0;    // Replace with your code
  static constexpr uint64_t VOL_DOWN = 0x0;  // Replace with your code
  static constexpr uint64_t MUTE = 0x0;      // Replace with your code
}

class IRReceiver {
private:
  // LG TV remote codes
  static constexpr uint32_t IR_CODE_LG_VOL_UP = 0x20DF40BF;
  static constexpr uint32_t IR_CODE_LG_VOL_DOWN = 0x20DFC03F;
  static constexpr uint32_t IR_CODE_LG_MUTE = 0x20DF906F;
  
  // Samsung TV remote codes (including Insignia NS-RMTSAM21)
  static constexpr uint32_t IR_CODE_SAMSUNG_VOL_UP = 0xE0E0E01F;
  static constexpr uint32_t IR_CODE_SAMSUNG_VOL_DOWN = 0xE0E0D02F;
  static constexpr uint32_t IR_CODE_SAMSUNG_MUTE = 0xE0E0F00F;
  
  IRrecv _recv;
  decode_results _res;
  IRCommand _last = IRCommand::None;
  
  IRCommand decode(uint64_t val) const {
    // Standard remotes (32-bit)
    if (val == IR_CODE_LG_VOL_UP || val == IR_CODE_SAMSUNG_VOL_UP) return IRCommand::VolumeUp;
    if (val == IR_CODE_LG_VOL_DOWN || val == IR_CODE_SAMSUNG_VOL_DOWN) return IRCommand::VolumeDown;
    if (val == IR_CODE_LG_MUTE || val == IR_CODE_SAMSUNG_MUTE) return IRCommand::Mute;
    
    // Your custom remote (50-bit UNKNOWN protocol)
    if (RawIR::VOL_UP != 0 && val == RawIR::VOL_UP) return IRCommand::VolumeUp;
    if (RawIR::VOL_DOWN != 0 && val == RawIR::VOL_DOWN) return IRCommand::VolumeDown;
    if (RawIR::MUTE != 0 && val == RawIR::MUTE) return IRCommand::Mute;
    
    return IRCommand::None;
  }

public:
  IRReceiver(uint8_t pin) : _recv(pin) {}
  
  void begin() { _recv.enableIRIn(); }
  
  IRCommand check() {
    if (!_recv.decode(&_res)) return IRCommand::None;
    
    // Filter invalid values
    if (_res.value == 0xFFFFFFFF || _res.value == 0 || _res.bits < 8) {
      _recv.resume();
      return IRCommand::None;
    }
    
    // Log ALL signals (including UNKNOWN) to capture your remote's codes
    Logger::logIR(typeToString(_res.decode_type), _res.value, _res.bits, _res.repeat);
    
    // Skip repeats for command detection
    if (_res.repeat) {
      IRCommand cmd = _last;
      if (cmd != IRCommand::None) {
        Logger::logIRCommand(toString(cmd) + " [repeat]");
      }
      _recv.resume();
      return cmd;
    }
    
    // Try to decode
    IRCommand cmd = decode(_res.value);
    
    if (cmd != IRCommand::None) {
      Logger::logIRCommand(toString(cmd));
      _last = cmd;
    } else {
      Logger::logIRUnmapped();
      Logger::logf("  👉 Copy this line to RawIR namespace:\n");
      Logger::logf("  static constexpr uint64_t YOUR_BUTTON = 0x%llX;\n", (unsigned long long)_res.value);
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
