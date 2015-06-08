#include "Light.h"




Light::Light(int pin) {
  m_pin = pin;
  pinMode(m_pin, OUTPUT);
}

Light::~Light() { }

void Light::on() {
  digitalWrite(m_pin, HIGH);
}

void Light::off() {
  digitalWrite(m_pin, LOW);
}

void Light::pulseStep() {
  if (m_up) {
    m_val = m_val + 10;
  } else {
    m_val = m_val - 10;
  }
  if (m_val > 255) {
    m_val = 255;
    m_up = false;
  }
  if (m_val < 0) {
    m_val = 0;
    m_up = true;
  }
  analogWrite(m_pin, m_val);
}
