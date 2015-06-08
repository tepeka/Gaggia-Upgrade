#include <PlayingWithFusion_MAX31865.h>
#include <SPI.h>
#include <PID_v1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "Light.h"
#include "RTD.h"
#include "GaggiaPID.h"
#include "Poti.h"

// -- pins
#define LED_PWM_PIN   13
#define RELAY_DIG_PIN 12
#define RTD_PWM_PIN    9
#define POTI_ANA_PIN  A0
#define OLED_MOSI      5
#define OLED_CLK      10
#define OLED_DC       11
#define OLED_CS       12
#define OLED_RESET     6

// -- rtd & temp
const int PROGMEM INVALID_TEMP = -274; // °C
const int PROGMEM TEMP_MEM_SIZE = 10;
const int PROGMEM TEMP_THRESHOLD = 0; // °C
RTD rtd(RTD_PWM_PIN);
int tempMem[TEMP_MEM_SIZE];
int tempMemIdx = 0;
int tempMemSum = 0;
int tempMemAvg = 0;

// -- led
Light light(LED_PWM_PIN);
bool led_on = true;
bool led_pulse = true;

// -- pid
const int PROGMEM SETPOINT_MIN = 90; // °C
const int PROGMEM SETPOINT_MAX = 110; // °C
const int PROGMEM SETPOINT_INIT = 100; // °C
const int PROGMEM WINDOW_SIZE = 2000;
const int P = 190;
const int I = 30;
const int D = 10;
GaggiaPID pid(SETPOINT_INIT, P, I, D, WINDOW_SIZE);

// -- poti
Poti poti(POTI_ANA_PIN, SETPOINT_MIN, SETPOINT_MAX);

// -- display
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void doInit() {
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
  if (tempMemAvg < pid.GetSetpoint() - TEMP_THRESHOLD) led_pulse = true;
  else if (tempMemAvg > pid.GetSetpoint() + TEMP_THRESHOLD) led_pulse = true;
  else led_pulse = false;
  led_on = true;
}

void doCalcPid() {
  int setpoint = poti.GetValue();
  pid.UpdateSetpoint(setpoint);
  bool on = pid.Calculate(tempMemAvg);
  //Serial.print(tempMemAvg);
  //Serial.print("deg C, relay: ");
  if (on) {
    digitalWrite(RELAY_DIG_PIN, HIGH);
    //Serial.print(HIGH);
  } else {
    digitalWrite(RELAY_DIG_PIN, LOW);
    //Serial.print(LOW);
  }
  //Serial.print(", ");
  //Serial.print(setpoint);
  //Serial.println();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temperature: " + String(tempMemAvg));
  display.println("Setpoint:    " + String(setpoint));
  if (on) {
    display.println("Heater:      on");
  } else {
    display.println("Heater:      off");
  }
  display.display();
}

void doSendHttp() {
  /*
   String path = "/CMD?BoilerSetpoint=" + pid.GetSetpoint();
   path = path + "&BoilerTemp=";
   path = path + tempMemAvg;
   // http.Get("192.168.70.14", path);
   if (client.connect("192.168.70.14", 8080)) {
     Serial.println("connected");
     // Make a HTTP request:
     client.println("GET " + path + " HTTP/1.1");
     client.println("Connection: close");
     client.println();
   }
   else {
     // kf you didn't get a connection to the server:
     Serial.println("connection failed");
   }
   if (client.available()) {
     char c = client.read();
     Serial.print(c);
   }
   if (!client.connected()) {
     Serial.println();
     Serial.println("disconnecting.");
     client.stop();
   }
  */
}
