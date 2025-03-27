#ifndef Poti_h
#define Poti_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Poti {
  public:
    Poti(short pin, short min, short max);
    ~Poti();
    short GetValue();

  private:
    short m_pin;
    short m_min;
    short m_max;

};

#endif
