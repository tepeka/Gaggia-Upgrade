#include "Poti.h"

Poti::Poti(int pin, double min, double max) {
  m_pin = pin;
  m_min = min;
  m_max = max;
  if (max < min) {
    m_min = max;
    m_max = min;
  }
}

Poti::~Poti() { }


double Poti::GetValue()
{
  int sensorValue = analogRead(m_pin); // 0..1023
  double scale = sensorValue / 1023.0;
  return m_min + (m_max - m_min) * scale;
}
