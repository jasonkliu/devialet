#pragma once
#include <M5Unified.h>
#include <vector>
#include "IRReceiver.h"

struct SpeakerDisplayInfo {
  char role;     // 'M', 'L', 'R'
  int volume;
  String ip;
};

class Display {
private:
  unsigned long _timeout = 0;
  
  uint16_t getVolumeColor(int vol) {
    if (vol < 35) return TFT_DARKGREY;
    if (vol < 70) return TFT_ORANGE;
    return TFT_RED;
  }

public:
  void begin() {
    M5.begin();
    M5.Display.setRotation(M5.Display.width() < M5.Display.height() ? 1 : 3);
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE, BLACK);
  }
  
  void update() { M5.update(); }
  
  void showMainScreen(const std::vector<SpeakerDisplayInfo>& speakers) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(4);
    M5.Display.setCursor(0, 0);
    
    if (speakers.empty()) {
      M5.Display.setTextSize(2);
      M5.Display.println("No speakers");
    } else {
      // Show volumes in big font
      for (const auto& sp : speakers) {
        M5.Display.setTextColor(getVolumeColor(sp.volume), BLACK);
        M5.Display.print(sp.role);
        M5.Display.print(": ");
        M5.Display.print(sp.volume);
        M5.Display.println("%");
      }
      
      // IP addresses at bottom for debugging
      M5.Display.println();
      M5.Display.setTextSize(1);
      M5.Display.setTextColor(TFT_DARKGREY, BLACK);
      M5.Display.print("(");
      for (size_t i = 0; i < speakers.size(); i++) {
        M5.Display.print(speakers[i].ip);
        if (i < speakers.size() - 1) M5.Display.print(", ");
      }
      M5.Display.print(")");
    }
    _timeout = 0;
  }
  
  void showCommand(const String& text, bool unknown = false) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(2);
    M5.Display.setCursor(0, 20);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.println(text);
    _timeout = millis() + (unknown ? 3500 : 500);
  }
  
  void showMessage(const String& text) {
    M5.Display.fillScreen(BLACK);
    M5.Display.setTextSize(1);
    M5.Display.setCursor(0, 0);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.println(text);
  }
  
  bool shouldRestoreMainScreen() { return _timeout && millis() >= _timeout; }
};
