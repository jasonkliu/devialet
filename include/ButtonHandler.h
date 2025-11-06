#pragma once
#include <M5Unified.h>
#include "Display.h"
#include "Speaker.h"
#include "IRReceiver.h"
#include "Logger.h"

class ButtonHandler {
private:
  Display& _display;
  SpeakerManager& _speakers;
  IRReceiver& _ir;

  void handle(IRCommand cmd, const String& name) {
    if (_speakers.count() == 0) {
      _display.showCommand("No speakers");
      return;
    }
    
    int ok = _speakers.executeAll(cmd);
    Logger::logf("%s: %d/%d OK\n", name.c_str(), ok, _speakers.count());
    _speakers.refreshVolumes();
    updateScreen();
  }

public:
  ButtonHandler(Display& d, SpeakerManager& s, IRReceiver& i) : _display(d), _speakers(s), _ir(i) {}
  
  void checkButtons() {
    if (M5.BtnA.wasPressed()) handle(IRCommand::VolumeUp, "VOL UP");
    else if (M5.BtnB.wasPressed()) handle(IRCommand::VolumeDown, "VOL DOWN");
  }
  
  void checkIR() {
    String debug;
    IRCommand cmd = _ir.checkForCommand(&debug);
    
    if (cmd != IRCommand::None) {
      handle(cmd, IRReceiver::commandToString(cmd));
    } else if (!debug.isEmpty()) {
      _display.showCommand("UNKNOWN\n" + debug, true);
    }
  }
  
  void updateScreen() {
    _display.showMainScreen(_speakers.getInfo());
  }
};
