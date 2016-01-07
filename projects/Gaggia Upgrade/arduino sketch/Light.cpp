#include "Light.h"




Light::Light(short pin) {
  m_pin = pin;
  pinMode(m_pin, OUTPUT);
}

Light::~Light() { }

void Light::on() {
  m_up = false;
  m_val = 255;
  analogWrite(m_pin, m_val);
}

void Light::off() {
  m_up = true;
  m_val = 0;
  analogWrite(m_pin, m_val);
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
