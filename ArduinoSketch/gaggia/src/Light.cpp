#include "Light.h"

Light::Light(short pin) {
  m_pin = pin;
  pinMode(m_pin, OUTPUT);
}

Light::~Light() { }

void Light::On() {
  On(100);
}

void Light::On(short dim) {
  m_pulse_val = m_dim_val = map(dim, 0, 100, 0, 255);
  m_pulse = false;
  m_pulse_up = false;
  analogWrite(m_pin, m_dim_val);
}

void Light::Off() {
  m_pulse_val = m_dim_val = 0;
  m_pulse = false;
  m_pulse_up = true;
  analogWrite(m_pin, m_dim_val);
}

void Light::PulseOn() {
  PulseOn(m_dim_val);
}

void Light::PulseOn(short dim) {
  if (!m_pulse) {
    m_pulse = true;
    m_dim_val = dim;
  }
}

void Light::HandlePulse() {
  if (m_pulse) {
    if (m_pulse_up) {
      m_pulse_val += 10;
    } else {
      m_pulse_val -= 10;
    }
    if (m_pulse_val > m_dim_val) {
      m_pulse_val = m_dim_val;
      m_pulse_up = false;
    }
    if (m_pulse_val < 0) {
      m_pulse_val = 0;
      m_pulse_up = true;
    }
    analogWrite(m_pin, m_pulse_val);
  }
}
