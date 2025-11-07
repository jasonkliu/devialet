#pragma once
#include <M5Unified.h>
#include <vector>

struct SpeakerDisplayInfo {
  char role;
  int volume;
  String ip;
};

class Display {
private:
  unsigned long _lastActivity = 0;
  bool _screenOn = true;
  bool _setupComplete = false;
  static constexpr unsigned long SLEEP_MS = 5000;
  
  uint16_t volumeColor(int vol) const {
    if (vol < 35) return TFT_DARKGREY;
    if (vol < 70) return TFT_ORANGE;
    return TFT_RED;
  }
  
  void wake() {
    if (!_screenOn) {
      M5.Display.wakeup();
      M5.Display.setBrightness(128);
      _screenOn = true;
    }
    _lastActivity = millis();
  }

public:
  void begin() {
    M5.begin();
    M5.Display.setRotation(M5.Display.width() < M5.Display.height() ? 1 : 3);
    M5.Display.fillScreen(BLACK);
    _lastActivity = millis();
  }
  
  void update() { 
    M5.update();
    if (_setupComplete && _screenOn && (millis() - _lastActivity) > SLEEP_MS) {
      M5.Display.sleep();
      M5.Display.setBrightness(0);
      _screenOn = false;
    }
  }
  
  void markSetupComplete() { _setupComplete = true; }
  void recordActivity() { wake(); }
  
  void showSpeakers(const std::vector<SpeakerDisplayInfo>& speakers) {
    wake();
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(0, 0);
    
    if (speakers.empty()) {
      M5.Display.setTextSize(2);
      M5.Display.setTextColor(WHITE);
      M5.Display.println("No speakers");
      return;
    }
    
    for (const auto& sp : speakers) {
      M5.Display.setTextSize(4);
      M5.Display.setTextColor(volumeColor(sp.volume));
      M5.Display.printf("%c: %d%%", sp.role, sp.volume);
      M5.Display.setTextSize(1);
      M5.Display.setTextColor(TFT_DARKGREY);
      M5.Display.printf(" %s", sp.ip.c_str());
      M5.Display.setTextSize(4);
      M5.Display.println();
    }
  }
  
  void showMessage(const String& text) {
    wake();
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(0, 0);
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(WHITE);
    M5.Display.println(text);
  }
};
