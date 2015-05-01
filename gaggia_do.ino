
RTD rtd(RTD_PWM_PIN);
const int INVALID_TEMP = -274; // °C
const int TEMP_MEM_SIZE = 10;

Light light(LED_PWM_PIN);
bool led_on = true;
bool led_pulse = true;

int tempMem[TEMP_MEM_SIZE];
int tempMemIdx = 0;
int tempMemSum = 0;
int tempMemAvg = 0;

double Setpoint, Input, Output;
PID pidCtrl(&Input, &Output, &Setpoint, 200, 50, 0, DIRECT);
int WindowSize = 2000;
unsigned long windowStartTime;


void doInit() {
  windowStartTime = millis();
  Setpoint = 90;
  pidCtrl.SetOutputLimits(0, WindowSize);
  pidCtrl.SetSampleTime(100);
  pidCtrl.SetMode(AUTOMATIC);
  tempMem[0] = INVALID_TEMP;
}

void doLedHandling() {
  if (!led_on) {
    light.off();

  } else if (led_pulse) {
    light.pulseStep();

  } else {
    light.on();
  }
}

void doReadTemp() {
  // -- calc avg temp
  int temp = rtd.readTemp();
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
  pidCtrl.Compute();
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


