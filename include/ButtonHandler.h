#pragma once
#include <M5Unified.h>
#include "Display.h"
#include "Speaker.h"
#include "IRReceiver.h"

class ButtonHandler {
private:
  Display& _display;
  SpeakerManager& _speakers;
  IRReceiver& _ir;

  void handle(IRCommand cmd) {
    if (_speakers.count() == 0) {
      _display.showMessage("No speakers");
      return;
    }
    
    _speakers.executeAll(cmd);
    _display.showSpeakers(_speakers.info());
  }

public:
  ButtonHandler(Display& d, SpeakerManager& s, IRReceiver& i) : _display(d), _speakers(s), _ir(i) {}
  
  void checkButtons() {
    if (M5.BtnA.wasPressed()) {
      _display.recordActivity();
      handle(IRCommand::VolumeUp);
    } else if (M5.BtnB.wasPressed()) {
      _display.recordActivity();
      handle(IRCommand::VolumeDown);
    }
  }
  
  void checkIR() {
    IRCommand cmd = _ir.check();
    if (cmd != IRCommand::None) {
      _display.recordActivity();
      handle(cmd);
    }
  }
  
  void updateScreen() {
    _display.showSpeakers(_speakers.info());
  }
};
