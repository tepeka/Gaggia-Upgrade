#ifndef GaggiaPID_h
#define GaggiaPID_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <PID_v1.h>

class GaggiaPID {
  public:
    GaggiaPID(short setpoint, short p, short i, short d, short windowSize);
    ~GaggiaPID();
    bool Calculate(short temperature);
    void UpdateSetpoint(short setpoint);
    short GetSetpoint();
    short GetOutput();
    
  private:
    PID* m_pid;
    short m_setpoint;
    short m_input;
    short m_output;
    short m_p;
    short m_i;
    short m_d;
    short m_windowSize;
    unsigned long m_windowStartTime;
    
};

#endif
