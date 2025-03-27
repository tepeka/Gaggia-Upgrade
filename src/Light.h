#ifndef Light_h
#define Light_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Light {
  public:
    Light(short pin);
    ~Light();
    void On();
    void On(short dim);
    void Off();
    void PulseOn();
    void PulseOn(short dim);
    
    void HandlePulse();
    
  private:
    
    short m_pin;
    short m_dim_val = 100;
    
    short m_pulse_val = 0;
    bool m_pulse = false;
    bool m_pulse_up = true;
};

#endif

