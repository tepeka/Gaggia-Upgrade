#include "GaggiaPID.h"



GaggiaPID::GaggiaPID(double setpoint, double p, double i, double d, unsigned long sampleTime, double windowSize) {
  m_setpoint = setpoint;
  m_windowSize = windowSize;
  m_p = p;
  m_i = i;
  m_d = d;
  m_pid = new PID(&m_input, &m_output, &m_setpoint, p, i, d, DIRECT);
  m_pid->SetOutputLimits(0, m_windowSize);
  m_pid->SetSampleTime(sampleTime);
  m_pid->SetMode(AUTOMATIC);
  m_windowStartTime = millis();
}

GaggiaPID::~GaggiaPID() {
  delete m_pid;
}

void GaggiaPID::UpdateSetpoint(double setpoint) {
  m_setpoint = setpoint;
}

double GaggiaPID::GetSetpoint() {
  return m_setpoint;
}

double GaggiaPID::GetHeatPercentage() {
  return 100 * m_output / m_windowSize;
}

bool GaggiaPID::Calculate(double temperature) {
  // set input and compute
  m_input = temperature;
  m_pid->Compute();
  // calculate current window time
  unsigned long now = millis();
  short windowTime = now - m_windowStartTime;
  if (windowTime > m_windowSize)
  {
    m_windowStartTime = now;
    windowTime = 0;
  }
  // return on/off based on current window time and output time ("on" time)
  if (windowTime <= m_output) {
    return true;
  } else {
    return false;
  }
}
