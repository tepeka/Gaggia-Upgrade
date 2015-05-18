#include <PlayingWithFusion_MAX31865.h>
#include <SPI.h>
#include <PID_v1.h>
#include <stdlib.h>
#include <string.h>
#include <Ethernet.h>

#include "Light.h"
#include "RTD.h"
#include "GaggiaPID.h"
#include "Poti.h"
//# include "HttpWrapper.h"

RTD rtd(RTD_PWM_PIN);
const int INVALID_TEMP = -274; // °C
const int TEMP_MEM_SIZE = 10;
const int TEMP_THRESHOLD = 0; // °C

Light light(LED_PWM_PIN);
bool led_on = true;
bool led_pulse = true;

int tempMem[TEMP_MEM_SIZE];
int tempMemIdx = 0;
int tempMemSum = 0;
int tempMemAvg = 0;

const int SETPOINT_MIN = 20; // °C
const int SETPOINT_MAX = 50; // °C
const int SETPOINT_INIT = 40; // °C
const int WINDOW_SIZE = 2000;
const int P = 190;
const int I = 30;
const int D = 10;
GaggiaPID pid(SETPOINT_INIT, P, I, D, WINDOW_SIZE);
Poti poti(POTI_ANA_PIN, SETPOINT_MIN, SETPOINT_MAX);

// HttpWrapper http;
IPAddress ip(192, 168, 70, 56);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;

void doInit() {
  tempMem[0] = INVALID_TEMP;
  pinMode(RELAY_DIG_PIN, OUTPUT);
  Ethernet.begin(mac, ip);
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
  Serial.print(tempMemAvg);
  Serial.print("deg C, relay: ");
  if (on) {
    digitalWrite(RELAY_DIG_PIN, HIGH);
    Serial.print(HIGH);
  } else {
    digitalWrite(RELAY_DIG_PIN, LOW);
    Serial.print(LOW);
  }
  Serial.print(", ");
  Serial.print(setpoint);
  Serial.println();
}

void doSendHttp() {
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
}

