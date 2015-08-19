#include <PlayingWithFusion_MAX31865.h>
#include <SPI.h>
#include <PID_v1.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
// #include <MemoryFree.h>

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
#define LED2_PWM_PIN   3
#define RELAY_DIG_PIN  4
#define OLED_CLK       5
#define OLED_MOSI      6
#define OLED_CS        7
#define OLED_DC        8
#define LED1_PWM_PIN   9
#define RTD_SS_PIN    10
// PIN 11/12/13: SPI (RTD)


// -- rtd & temp
const int INVALID_TEMP = -274; // °C
const int TEMP_MEM_SIZE = 10;
const int TEMP_THRESHOLD = 0; // °C
RTD rtd(RTD_SS_PIN);
int tempMem[TEMP_MEM_SIZE];
int tempMemIdx = 0;
int tempMemSum = 0;
int tempMemAvg = 0;

// -- led
Light light(LED1_PWM_PIN);
bool led_on = true;
bool led_pulse = true;

// -- pid
const int SETPOINT_MIN = 20; // °C
const int SETPOINT_MAX = 40; // °C
const int SETPOINT_INIT = 25; // °C
const int WINDOW_SIZE = 2000;
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
  Serial.print(F("temp: "));
  Serial.print(tempMemAvg);
  Serial.print(F(", relay: "));
  if (on) {
    digitalWrite(RELAY_DIG_PIN, HIGH);
    Serial.print(HIGH);
  } else {
    digitalWrite(RELAY_DIG_PIN, LOW);
    Serial.print(LOW);
  }
  Serial.print(F(", setpoint: "));
  Serial.print(setpoint);
  //Serial.print(F(", freeMemory: "));
  //Serial.print(freeMemory());
  Serial.println();
  
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
