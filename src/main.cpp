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
  delay(100); // Let serial stabilize
  Logger::logln("\n\n=== DEVIALET IR STARTING ===");
  Logger::logf("IR Pin: %d\n", IR_PIN);
  
  display.begin();
  display.showMessage("Devialet IR\nStarting...");
  
  Logger::logln("Initializing IR receiver...");
  ir.begin();
  Logger::logln("IR receiver ready - point remote and press buttons");
  
  display.showMessage("WiFi\nConnecting...");
  wifi.connectBlocking(); // Blocking is OK during setup
  
  if (wifi.isConnected()) {
    display.showMessage("WiFi OK\nFinding speakers...");
    speakers.discover(wifi);
    speakers.refresh();
    
    if (speakers.hasValidSpeakers()) {
      Logger::logln("Ready");
    } else {
      Logger::logln("Warning: No valid speakers found");
    }
  } else {
    display.showMessage("WiFi failed\nContinuing anyway...");
    delay(2000);
  }
  
  buttons.updateScreen();
  display.markSetupComplete();
}

void loop() {
  // Priority 1: Process IR input (critical path, lowest latency)
  buttons.checkIR();
  
  // Priority 2: Process physical buttons
  buttons.checkButtons();
  
  // Priority 3: Update display (can tolerate latency)
  display.update();
  
  // Priority 4: Handle WiFi reconnection (non-blocking, throttled)
  if (!wifi.isConnected()) {
    wifi.connect(); // Non-blocking, throttled to 5s intervals
  }
  
  // Minimal loop delay for maximum IR responsiveness
  delay(10);
}
