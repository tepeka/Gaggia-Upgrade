#include "Poti.h"

Poti::Poti(short pin, short min, short max) {
  m_pin = pin;
  m_min = min;
  m_max = max;
  if (max < min) {
    m_min = max;
    m_max = min;
  }
}

Poti::~Poti() { }


short Poti::GetValue()
{
  short sensorValue = analogRead(m_pin); // 0..1023
  return map(sensorValue, 0, 1024, m_min, m_max);
}
