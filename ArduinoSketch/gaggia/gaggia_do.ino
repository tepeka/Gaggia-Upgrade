#include <SPI.h>
#include <Wire.h>

#include "libs/Adafruit_GFX/Adafruit_GFX.h"
#include "libs/Adafruit_SSD1306/Adafruit_SSD1306.h"
#include "libs/MemoryFree/MemoryFree.h"
#include "libs/PID_v1/PID_v1.h"
#include "libs/PlayingWithFusion_MAX31865/PlayingWithFusion_MAX31865.h"

#include "Light.h"
#include "RTD.h"
#include "GaggiaPID.h"
#include "Poti.h"

#if (SSD1306_LCDHEIGHT != 64)
#error("Display height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// -- pins
#define OLED_RESET    -1
#define POTI_ANA_PIN  A0
#define LED1_PWM_PIN   3
#define RELAY_DIG_PIN  4
#define OLED_CLK       5
#define OLED_MOSI      6
#define OLED_CS        7
#define OLED_DC        8
#define LED2_PWM_PIN   9
#define RTD_SS_PIN    10
// PIN 11/12/13: SPI (RTD)


// -- rtd & temp
const short INVALID_TEMP = -274; // °C
const short TEMP_MEM_SIZE = 10;
const short TEMP_THRESHOLD = 1; // °C
RTD rtd(RTD_SS_PIN);
short tempMem[TEMP_MEM_SIZE];
short tempMemIdx = 0;
short tempMemSum = 0;
short tempMemAvg = 0;

// -- led
Light light1(LED1_PWM_PIN);
Light light2(LED2_PWM_PIN);
bool led_on = true;
bool led_pulse = true;

// -- pid
const short SETPOINT_MIN = 95; // °C
const short SETPOINT_MAX = 125; // °C
const short SETPOINT_INIT = 110; // °C
const short WINDOW_SIZE = 2000;
const short P = 270;
const short I = 30;
const short D = 10;
GaggiaPID pid(SETPOINT_INIT, P, I, D, WINDOW_SIZE);

// -- poti
Poti poti(POTI_ANA_PIN, SETPOINT_MIN, SETPOINT_MAX);

// -- display
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void doInit() {
  light1.on();
  tempMem[0] = INVALID_TEMP;
  pinMode(RELAY_DIG_PIN, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.drawBitmap(0, 0,  GAGGIA_PIC, 128, 64, 1);
  display.display();
  delay(5000);
  display.clearDisplay();
}

void doLedHandling() {
  if (!led_on) {
    light2.off();

  } else if (led_pulse) {
    light2.pulseStep();

  } else {
    light2.on();
  }
}

void doReadTemp() {
  // -- calc avg temp
  short temp = rtd.readTemp();
  if (tempMem[0] == INVALID_TEMP) {
    // initialize
    for (short i = 0; i < TEMP_MEM_SIZE; i++) {
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
  if(SETPOINT_MIN == pid.GetSetpoint()) led_pulse = false;
  else if (tempMemAvg < pid.GetSetpoint() - TEMP_THRESHOLD) led_pulse = true;
  else if (tempMemAvg > pid.GetSetpoint() + TEMP_THRESHOLD) led_pulse = true;
  else led_pulse = false;
  led_on = true;
}

void doCalcPid() {
  short setpoint = poti.GetValue();
  pid.UpdateSetpoint(setpoint);
  bool on = pid.Calculate(tempMemAvg);
  if (on) {
    digitalWrite(RELAY_DIG_PIN, HIGH);
  } else {
    digitalWrite(RELAY_DIG_PIN, LOW);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print(F("Temperature: "));
  display.println(String(tempMemAvg));
  display.print(F("Setpoint:    "));
  display.println(String(setpoint));
  if (on) {
    display.println(F("Heater:      on"));
  } else {
    display.println(F("Heater:      off"));
  }
  display.print(F("Output:      "));
  display.println(String(pid.GetOutput()));
  display.print(F("Mem:         "));
  display.println(String(freeMemory()));
  display.display();
}

void doSendHttp() {
  if (Serial.available()) {
    short setpoint = poti.GetValue();
    Serial.print(F("msg0,"));
    Serial.print(tempMemAvg);
    Serial.print(F(","));
    Serial.print(setpoint);
    Serial.print(F(","));
    Serial.print(tempMemAvg - setpoint);
    Serial.println();
  }
}

