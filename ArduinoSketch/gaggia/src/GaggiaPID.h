#ifndef GaggiaPID_h
#define GaggiaPID_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "libs/PID_v1/PID_v1.h"


class GaggiaPID {
  public:
    GaggiaPID(double setpoint, double p, double i, double d, unsigned long sampleTime, double windowSize);
    ~GaggiaPID();
    bool Calculate(double temperature);
    void UpdateSetpoint(double setpoint);
    double GetSetpoint();
    double GetHeatPercentage();
    
  private:
    PID* m_pid;
    double m_setpoint;
    double m_input;
    double m_output;
    double m_p;
    double m_i;
    double m_d;
    double m_windowSize;
    unsigned long m_windowStartTime;
    
};

#endif
