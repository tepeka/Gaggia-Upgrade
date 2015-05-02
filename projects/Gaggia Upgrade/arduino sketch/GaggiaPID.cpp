#include "GaggiaPID.h"



GaggiaPID::GaggiaPID(int setpoint, int p, int i, int d, int windowSize) {
  m_setpoint = setpoint;
  m_windowSize = windowSize;
  m_p = p;
  m_i = i;
  m_d = d;
  m_pid = new PID(&m_input, &m_output, &m_setpoint, p, i, d, DIRECT);
  m_pid->SetOutputLimits(0, m_windowSize);
  m_pid->SetSampleTime(100);
  m_pid->SetMode(AUTOMATIC);
  m_windowStartTime = millis();
}

GaggiaPID::~GaggiaPID() { 
  delete m_pid;
}


bool GaggiaPID::Calculate(int temperature) {
  m_input = temperature;
  m_pid->Compute();
  if (millis() - m_windowStartTime > m_windowSize)
  {
    m_windowStartTime += m_windowSize;
  }
  if (m_output < millis() - m_windowStartTime) {
    return true;
  } else {
    return false;
  }  
}
