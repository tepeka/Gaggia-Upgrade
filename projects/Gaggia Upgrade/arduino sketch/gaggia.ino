#include <PlayingWithFusion_MAX31865.h>
#include <SPI.h>
#include <PID_v1.h>

#include "Light.h"
#include "RTD.h"
#include "GaggiaPID.h"

const int LED_PWM_PIN = 13;
const int RELAY_PWM_PIN = 12;
const int RTD_PWM_PIN = 9;

const int LED_HANDLING_INTERVAL = 20;
const int TEMP_READ_INTERVAL = 100;
const int PID_CALC_INTERVAL = 1;

unsigned long lastLedHandling = 0;
unsigned long lastTempRead = 0;
unsigned long lastPidCalc = 0;

void setup() {
  Serial.begin(115200);
  doInit();
}

void loop() {
  unsigned long now = millis();
  // -- execute led pulse
  if ((now - lastLedHandling) / LED_HANDLING_INTERVAL >= 1) {
    doLedHandling();
    lastLedHandling = millis();
  }
  // -- execute read temp
  if ((now - lastTempRead) / TEMP_READ_INTERVAL >= 1) {
    doReadTemp();
    lastTempRead = millis();
  }
  // -- execute pid calc
  if ((now - lastPidCalc) / PID_CALC_INTERVAL >= 1) {
    doCalcPid();
    lastPidCalc = millis();
  }
}
