const int LED_PWM_PIN = 13;
const int RELAY_DIG_PIN = 12;
const int RTD_PWM_PIN = 9;
const int POTI_ANA_PIN = A0;

const int LED_HANDLING_INTERVAL = 20;
const int TEMP_READ_INTERVAL = 50;
const int PID_CALC_INTERVAL = 1;
const int SEND_HTTP_INTERVAL = 2000;

unsigned long lastLedHandling = 0;
unsigned long lastTempRead = 0;
unsigned long lastPidCalc = 0;
unsigned long lastSendHttp = 0;

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
  // -- execute send http
  if ((now - lastSendHttp) / SEND_HTTP_INTERVAL >= 1) {
    doSendHttp();
    lastSendHttp = millis();
  }
}
