#include <Arduino.h>
#include "Secrets.h"
#include "Logger.h"
#include "Display.h"
#include "Connectivity.h"
#include "IRReceiver.h"
#include "Speaker.h"
#include "ButtonHandler.h"

constexpr uint8_t IR_PIN = 33;

Logger logger;
Display display;
Connectivity wifi(WIFI_SSID, WIFI_PASSWORD);
IRReceiver ir(IR_PIN);
SpeakerManager speakers;
ButtonHandler buttons(display, speakers, ir);

void setup() {
  logger.begin(115200);
  delay(100);
  
  Logger::info("Devialet IR Controller");
  display.begin();
  display.showMessage("Devialet IR\nStarting...");
  ir.begin();
  
  display.showMessage("WiFi\nConnecting...");
  wifi.connectBlocking();
  
  if (wifi.isConnected()) {
    display.showMessage("Discovering\nspeakers...");
    speakers.discover(wifi);
    speakers.refresh();
    
    if (speakers.hasValidSpeakers()) {
      Logger::info("Ready");
    } else {
      Logger::error("No valid speakers");
    }
  } else {
    display.showMessage("WiFi failed\nContinuing...");
    delay(2000);
  }
  
  buttons.updateScreen();
  display.markSetupComplete();
}

void loop() {
  buttons.checkIR();
  buttons.checkButtons();
  display.update();
  
  if (!wifi.isConnected()) {
    wifi.connect();
  }
  
  delay(10);
}
