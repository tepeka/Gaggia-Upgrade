#ifndef RTD_h
#define RTD_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>

#include "libs/PlayingWithFusion_MAX31865/PlayingWithFusion_MAX31865.h"
#include "libs/PlayingWithFusion_MAX31865/PlayingWithFusion_MAX31865_STRUCT.h"

class RTD {
  public:
    RTD(short pin);
    ~RTD();
    double ReadTemp();
    
  private:
    double m_currentTemp; 
    PWFusion_MAX31865_RTD* m_rtd;

};

#endif
