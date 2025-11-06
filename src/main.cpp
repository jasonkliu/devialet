#include <Arduino.h>

#include "Secrets.h"
#include "Logger.h"
#include "Display.h"
#include "Connectivity.h"
#include "IRReceiver.h"
#include "Speaker.h"
#include "ButtonHandler.h"

constexpr uint8_t IR_PIN = 26;

Logger logger;
Display display;
Connectivity wifi(WIFI_SSID, WIFI_PASSWORD);
IRReceiver ir(IR_PIN);
SpeakerManager speakers;
ButtonHandler buttons(display, speakers, ir);

void setup() {
  logger.begin(115200);
  display.begin();
  display.showMessage("Devialet IR\nStarting...");
  ir.begin();
  
  display.showMessage("WiFi\nConnecting...");
  wifi.connect();
  
  if (wifi.isConnected()) {
    display.showMessage("WiFi OK\nFinding speakers...");
    speakers.discover(wifi, true);
    speakers.refreshVolumes();
  }
  
  buttons.updateScreen();
}

void loop() {
  display.update();
  buttons.checkButtons();
  
  if (!wifi.isConnected()) {
    display.showMessage("WiFi lost\nReconnecting...");
    wifi.connect();
    delay(500);
    return;
  }
  
  if (display.shouldRestoreMainScreen()) {
    buttons.updateScreen();
  }
  
  buttons.checkIR();
  delay(20);
}
