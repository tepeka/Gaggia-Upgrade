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
double tempMem[TEMP_MEM_SIZE];
short tempMemIdx = 0;
double tempMemSum = 0;
double tempMemAvg = 0;

// -- led
Light light1(LED1_PWM_PIN);
Light light2(LED2_PWM_PIN);

// -- pid
const short SETPOINT_MIN = 100; // °C
const short SETPOINT_MAX = 120; // °C
const short SETPOINT_INIT = 110; // °C
const unsigned long SAMPLE_TIME = 500;
const double WINDOW_SIZE = 1000;
const double P = 60;
const double I = 1.2;
const double D = 10;
GaggiaPID pid(SETPOINT_INIT, P, I, D, SAMPLE_TIME, WINDOW_SIZE);

// -- poti
Poti poti(POTI_ANA_PIN, SETPOINT_MIN, SETPOINT_MAX);

// -- display
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void doInit() {
  light1.On(100);
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
  light1.HandlePulse();
  light2.HandlePulse();
}

void doReadTemp() {
  // -- calc avg temp
  double temp = rtd.ReadTemp();
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
  if (SETPOINT_MIN == pid.GetSetpoint()) light2.On(75);
  else if (tempMemAvg < pid.GetSetpoint() - TEMP_THRESHOLD) light2.PulseOn(75);
  else if (tempMemAvg > pid.GetSetpoint() + TEMP_THRESHOLD) light2.PulseOn(75);
  else light2.On(75);
}

void doCalcPid() {
  short setpoint = poti.GetValue();
  pid.UpdateSetpoint((double)setpoint);
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
  display.print(F("Heat:        "));
  display.print(String(pid.GetHeatPercentage()));
  display.println("%");
  display.print(F("Mem:         "));
  display.println(String(freeMemory()));
  display.display();
}

void doPlot() {
  short setpoint = poti.GetValue();
  // Serial.print(F("msg0,"));
  Serial.print(String(tempMemAvg));
  Serial.print(F(","));
  Serial.print(setpoint);
  Serial.print(F(","));
  Serial.print(setpoint + TEMP_THRESHOLD);
  Serial.print(F(","));
  Serial.print(setpoint - TEMP_THRESHOLD);
  Serial.println();
}

