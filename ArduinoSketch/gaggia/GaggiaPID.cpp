#include "GaggiaPID.h"



GaggiaPID::GaggiaPID(short setpoint, short p, short i, short d, short windowSize) {
  m_setpoint = setpoint;
  m_windowSize = windowSize;
  m_p = p;
  m_i = i;
  m_d = d;
  m_pid = new PID(&m_input, &m_output, &m_setpoint, p, i, d, DIRECT);
  m_pid->SetOutputLimits(0, m_windowSize);
  m_pid->SetSampleTime(500);
  m_pid->SetMode(AUTOMATIC);
  //m_pid->SetControllerDirection(REVERSE);
  m_windowStartTime = millis();
}

GaggiaPID::~GaggiaPID() {
  delete m_pid;
}

void GaggiaPID::UpdateSetpoint(short setpoint) {
  m_setpoint = setpoint;
}

short GaggiaPID::GetSetpoint() {
  return m_setpoint;
}

short GaggiaPID::GetOutput() {
  return m_output;
}

bool GaggiaPID::Calculate(short temperature) {
  m_input = temperature;
  m_pid->Compute();
  if (millis() - m_windowStartTime > m_windowSize)
  {
    m_windowStartTime += m_windowSize;
  }
  if (m_output > millis() - m_windowStartTime) {
    return true;
  } else {
    return false;
  }
}
