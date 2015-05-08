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
    GaggiaPID(int setpoint, int p, int i, int d, int windowSize);
    ~GaggiaPID();
    bool Calculate(int temperature);
    void UpdateSetpoint(int setpoint);
    int GetSetpoint();
    
  private:
    PID* m_pid;
    double m_setpoint;
    double m_input;
    double m_output;
    int m_p;
    int m_i;
    int m_d;
    int m_windowSize;
    unsigned long m_windowStartTime;
    
};

#endif
