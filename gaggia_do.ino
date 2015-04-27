#include <PID_v1.h>


const int INVALID_TEMP = -274; // °C
const int TEMP_MEM_SIZE = 10;

bool led_on = true;
bool led_pulse = true;

int tempMem[TEMP_MEM_SIZE];
int tempMemIdx = 0;
int tempMemSum = 0;
int tempMemAvg = 0;

double Setpoint, Input, Output;
PID myPID(&Input, &Output, &Setpoint, 200, 50, 0, DIRECT);
int WindowSize = 2000;
unsigned long windowStartTime;

void doInit() {
  windowStartTime = millis();
  Setpoint = 90;
  myPID.SetOutputLimits(0, WindowSize);
  myPID.SetSampleTime(100);
  myPID.SetMode(AUTOMATIC);
  tempMem[0] = INVALID_TEMP;
}

void doLedHandling() {
  if (!led_on) {
    ledOff();

  } else if (led_pulse) {
    ledPulseStep();

  } else {
    ledOn();
  }
}

void doReadTemp() {
  // -- calc avg temp
  int temp = rtdReadTemp();
  if (tempMem[0] == INVALID_TEMP) {
    // initialize
    for (int i = 0; i < TEMP_MEM_SIZE; i++) {
      tempMem[i] = tempMemAvg = temp;
      tempMemSum += temp;
    }
  } else {
    tempMemSum -= tempMem[tempMemIdx];
    tempMem[tempMemIdx] = temp;
    tempMemSum += tempMem[tempMemIdx];
    tempMemIdx = (tempMemIdx + 1) % TEMP_MEM_SIZE;
    tempMemAvg = tempMemSum / TEMP_MEM_SIZE;
  }
  // -- set led state depending on temp
  if (tempMemAvg < Setpoint - 2) led_pulse = true;
  else if (tempMemAvg > Setpoint + 2) led_pulse = true;
  else led_pulse = false;
  led_on = true;
}

void doCalcPid() {
  Input = tempMemAvg;
  myPID.Compute();
  if (millis() - windowStartTime > WindowSize)
  {
    windowStartTime += WindowSize;
  }
  Serial.print(tempMemAvg);
  Serial.print("deg C, relay: ");
  if (Output < millis() - windowStartTime) {
    digitalWrite(RELAY_PWM_PIN, HIGH);
    Serial.println(HIGH);
  } else {
    digitalWrite(RELAY_PWM_PIN, LOW);
    Serial.println(LOW);
  }
}


