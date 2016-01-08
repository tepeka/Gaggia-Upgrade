#ifndef RTD_h
#define RTD_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>

#include "PlayingWithFusion_MAX31865.h"
#include "PlayingWithFusion_MAX31865_STRUCT.h"

class RTD {
  public:
    RTD(short pin);
    ~RTD();
    short readTemp();
    
  private:
    short m_currentTemp; 
    PWFusion_MAX31865_RTD* m_rtd;

};

#endif
