#ifndef Poti_h
#define Poti_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Poti {
  public:
    Poti(int pin, double min, double max);
    ~Poti();
    double GetValue();

  private:
    double m_pin;
    double m_min;
    double m_max;

};

#endif
