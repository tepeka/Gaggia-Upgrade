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
    void on();
    void off();
    void pulseStep();
    
  private:
    short m_val = 0;
    short m_pin;
    boolean m_up = true;
};

#endif
