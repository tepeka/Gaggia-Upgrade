#include <Arduino.h>

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
#define OLED_RESET -1
#define POTI_ANA_PIN A0
#define LED1_PWM_PIN 3
#define RELAY_DIG_PIN 4
#define OLED_CLK 5
#define OLED_MOSI 6
#define OLED_CS 7
#define OLED_DC 8
#define LED2_PWM_PIN 9
#define RTD_SS_PIN 10
// PIN 11/12/13: SPI (RTD)

static const unsigned char PROGMEM GAGGIA_PIC[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x03, 0xC3, 0x8B, 0x83, 0x88, 0x8F, 0x01, 0x70, 0x73, 0xC0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x04, 0xCC, 0x84, 0xC9, 0x11, 0x81, 0x88, 0x99, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x08, 0x48, 0x48, 0x4A, 0x00, 0x81, 0x09, 0x09, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x0F, 0xC8, 0x4F, 0xCE, 0x0F, 0x81, 0x09, 0xF9, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x08, 0x08, 0x48, 0x0A, 0x10, 0x81, 0x09, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x04, 0x4C, 0xC4, 0x49, 0x11, 0x91, 0x08, 0x89, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xC3, 0x8B, 0x83, 0x88, 0x8E, 0x91, 0x08, 0x70, 0xC0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1F, 0xFE, 0x00, 0x7F, 0x00, 0x00, 0xFF, 0xF0, 0x01, 0xFF, 0xE7, 0xF0, 0x07, 0xF0, 0xF0,
    0x00, 0x7F, 0xFE, 0x00, 0x7F, 0x00, 0x03, 0xFF, 0xF0, 0x07, 0xFF, 0xE7, 0xF0, 0x07, 0xF8, 0xF0,
    0x00, 0xFF, 0xFE, 0x00, 0xFF, 0x80, 0x0F, 0xFF, 0xF0, 0x1F, 0xFF, 0xE7, 0xF0, 0x07, 0xF8, 0xF0,
    0x03, 0xFF, 0xFE, 0x00, 0xFF, 0x80, 0x1F, 0xFF, 0xF0, 0x3F, 0xFF, 0xE7, 0xF0, 0x0F, 0xF8, 0xF0,
    0x07, 0xFF, 0xFE, 0x00, 0xFF, 0x80, 0x3F, 0xFF, 0xF0, 0x7F, 0xFF, 0xE7, 0xF0, 0x0F, 0xF8, 0x60,
    0x07, 0xFF, 0xFE, 0x00, 0xFF, 0x80, 0x7F, 0xFF, 0xF0, 0xFF, 0xFF, 0xE7, 0xF0, 0x0F, 0xFC, 0x00,
    0x0F, 0xFE, 0x00, 0x01, 0xFF, 0xC0, 0xFF, 0xE0, 0x00, 0xFF, 0xE0, 0x07, 0xF0, 0x0F, 0xFC, 0x00,
    0x1F, 0xF0, 0x00, 0x01, 0xFF, 0xC0, 0xFF, 0x80, 0x01, 0xFF, 0x00, 0x07, 0xF0, 0x1F, 0xFC, 0x00,
    0x1F, 0xE0, 0x00, 0x01, 0xFF, 0xC1, 0xFE, 0x00, 0x03, 0xFE, 0x00, 0x07, 0xF0, 0x1F, 0xFC, 0x00,
    0x3F, 0xC0, 0x00, 0x01, 0xFF, 0xE1, 0xFC, 0x00, 0x03, 0xFC, 0x00, 0x07, 0xF0, 0x1F, 0xFE, 0x00,
    0x3F, 0x80, 0x00, 0x03, 0xFF, 0xE3, 0xFC, 0x00, 0x03, 0xF8, 0x00, 0x07, 0xF0, 0x1F, 0xFE, 0x00,
    0x3F, 0x80, 0x00, 0x03, 0xFF, 0xE3, 0xF8, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xF0, 0x3F, 0xFE, 0x00,
    0x7F, 0x00, 0x00, 0x03, 0xFF, 0xE3, 0xF8, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xF0, 0x3F, 0xFF, 0x00,
    0x7F, 0x00, 0x00, 0x03, 0xFF, 0xF3, 0xF8, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xF0, 0x3F, 0xFF, 0x00,
    0x7F, 0x00, 0x00, 0x07, 0xFF, 0xF3, 0xF0, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xF0, 0x3F, 0xFF, 0x00,
    0x7F, 0x00, 0x3C, 0x07, 0xF7, 0xF3, 0xF0, 0x03, 0xE7, 0xF0, 0x07, 0xC7, 0xF0, 0x7F, 0x7F, 0x00,
    0x7F, 0x00, 0x7E, 0x07, 0xF7, 0xF3, 0xF8, 0x07, 0xF7, 0xF0, 0x0F, 0xE7, 0xF0, 0x7F, 0x3F, 0x80,
    0x7F, 0x00, 0x7E, 0x07, 0xE3, 0xFB, 0xF8, 0x07, 0xF7, 0xF0, 0x0F, 0xE7, 0xF0, 0x7F, 0x3F, 0x80,
    0x3F, 0x80, 0x7E, 0x0F, 0xE3, 0xFB, 0xF8, 0x07, 0xF7, 0xF0, 0x0F, 0xE7, 0xF0, 0xFF, 0x3F, 0x80,
    0x3F, 0x80, 0x7E, 0x0F, 0xFF, 0xFB, 0xFC, 0x07, 0xF3, 0xF8, 0x0F, 0xE7, 0xF0, 0xFF, 0xFF, 0x80,
    0x3F, 0xC0, 0x7E, 0x0F, 0xFF, 0xF9, 0xFC, 0x07, 0xF3, 0xF8, 0x0F, 0xE7, 0xF0, 0xFF, 0xFF, 0xC0,
    0x1F, 0xE0, 0x7E, 0x1F, 0xFF, 0xFD, 0xFE, 0x07, 0xF3, 0xFC, 0x0F, 0xE7, 0xF0, 0xFF, 0xFF, 0xC0,
    0x1F, 0xF0, 0x7E, 0x1F, 0xFF, 0xFD, 0xFF, 0x07, 0xF1, 0xFF, 0x0F, 0xE7, 0xF1, 0xFF, 0xFF, 0xC0,
    0x0F, 0xFC, 0xFE, 0x1F, 0xFF, 0xFC, 0xFF, 0xC7, 0xF1, 0xFF, 0xCF, 0xE7, 0xF1, 0xFF, 0xFF, 0xE0,
    0x0F, 0xFF, 0xFE, 0x1F, 0xFF, 0xFE, 0x7F, 0xFF, 0xF0, 0xFF, 0xFF, 0xE7, 0xF1, 0xFF, 0xFF, 0xE0,
    0x07, 0xFF, 0xFE, 0x3F, 0xFF, 0xFE, 0x3F, 0xFF, 0xF0, 0x7F, 0xFF, 0xE7, 0xF1, 0xFF, 0xFF, 0xE0,
    0x03, 0xFF, 0xFE, 0x3F, 0x80, 0xFE, 0x1F, 0xFF, 0xF0, 0x3F, 0xFF, 0xE7, 0xF3, 0xF8, 0x0F, 0xE0,
    0x01, 0xFF, 0xFE, 0x3F, 0x80, 0xFE, 0x0F, 0xFF, 0xF0, 0x1F, 0xFF, 0xE7, 0xF3, 0xF8, 0x07, 0xF0,
    0x00, 0x7F, 0xFE, 0x3F, 0x80, 0x7F, 0x07, 0xFF, 0xF0, 0x0F, 0xFF, 0xE7, 0xF3, 0xF8, 0x07, 0xF0,
    0x00, 0x1F, 0xFE, 0x7F, 0x00, 0x7F, 0x01, 0xFF, 0xF0, 0x03, 0xFF, 0xE7, 0xF3, 0xF8, 0x07, 0xF0,
    0x00, 0x03, 0xFE, 0x3F, 0x00, 0x7F, 0x00, 0x3F, 0xE0, 0x00, 0x3F, 0xE7, 0xE3, 0xF0, 0x03, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const short LED_HANDLING_INTERVAL = 20;
const short TEMP_READ_INTERVAL = 50;
const short PID_CALC_INTERVAL = 10;
const short PLOT_INTERVAL = 750;

unsigned long lastLedHandling = 0;
unsigned long lastTempRead = 0;
unsigned long lastPidCalc = 0;
unsigned long lastPlot = 0;


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

void doInit();
void doLedHandling();
void doReadTemp();
void doCalcPid();
void doPlot();

void setup()
{
  Serial.begin(9600);
  doInit();
}

void loop()
{
  unsigned long now = millis();
  // -- execute led pulse
  if (now - lastLedHandling > LED_HANDLING_INTERVAL)
  {
    doLedHandling();
    lastLedHandling = now;
  }
  // -- execute read temp
  if (now - lastTempRead > TEMP_READ_INTERVAL)
  {
    doReadTemp();
    lastTempRead = now;
  }
  // -- execute pid calc
  if (now - lastPidCalc > PID_CALC_INTERVAL)
  {
    doCalcPid();
    lastPidCalc = now;
  }
  // -- execute send http
  if (now - lastPlot > PLOT_INTERVAL)
  {
    doPlot();
    lastPlot = now;
  }
}

void doInit()
{
  light1.On(100);
  tempMem[0] = INVALID_TEMP;
  pinMode(RELAY_DIG_PIN, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.drawBitmap(0, 0, GAGGIA_PIC, 128, 64, 1);
  display.display();
  delay(5000);
  display.clearDisplay();
}

void doLedHandling()
{
  light1.HandlePulse();
  light2.HandlePulse();
}

void doReadTemp()
{
  // -- calc avg temp
  double temp = rtd.ReadTemp();
  if (tempMem[0] == INVALID_TEMP)
  {
    // initialize
    for (short i = 0; i < TEMP_MEM_SIZE; i++)
    {
      tempMem[i] = tempMemAvg = temp;
      tempMemSum += temp;
    }
  }
  else
  {
    tempMemSum -= tempMem[tempMemIdx];
    tempMem[tempMemIdx] = temp;
    tempMemSum += tempMem[tempMemIdx];
    tempMemIdx = (tempMemIdx + 1) % TEMP_MEM_SIZE;
    tempMemAvg = tempMemSum / TEMP_MEM_SIZE;
  }
  // -- set led state depending on temp
  if (SETPOINT_MIN == pid.GetSetpoint())
    light2.On(75);
  else if (tempMemAvg < pid.GetSetpoint() - TEMP_THRESHOLD)
    light2.PulseOn(75);
  else if (tempMemAvg > pid.GetSetpoint() + TEMP_THRESHOLD)
    light2.PulseOn(75);
  else
    light2.On(75);
}

void doCalcPid()
{
  short setpoint = poti.GetValue();
  pid.UpdateSetpoint((double)setpoint);
  bool on = pid.Calculate(tempMemAvg);
  if (on)
  {
    digitalWrite(RELAY_DIG_PIN, HIGH);
  }
  else
  {
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

void doPlot()
{
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
