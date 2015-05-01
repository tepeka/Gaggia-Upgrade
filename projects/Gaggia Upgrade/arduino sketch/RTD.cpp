#include "RTD.h"

RTD::RTD(int pin) {
  m_rtd = new PWFusion_MAX31865_RTD(pin);

  SPI.begin();                            // begin SPI
  SPI.setClockDivider(SPI_CLOCK_DIV16);   // SPI speed to SPI_CLOCK_DIV16 (1MHz)
  SPI.setDataMode(SPI_MODE3);             // MAX31865 works in MODE1 or MODE3

  pinMode(pin, OUTPUT);
  m_rtd->MAX31865_config();
}

RTD::~RTD() { 
  delete m_rtd;
}


int RTD::readTemp()
{
  bool error = false;
  char* msg;

  static struct var_max31865 RTD_CH0;
  double resistance;

  RTD_CH0.RTD_type = 1;                         // un-comment for PT100 RTD

  struct var_max31865 *rtd_ptr;
  rtd_ptr = &RTD_CH0;
  m_rtd->MAX31865_full_read(rtd_ptr);          // Update MAX31855 readings

  if (0 == RTD_CH0.status)
  {
    if (1 == RTD_CH0.RTD_type)
    {
      // calculate RTD resistance
      resistance = (double)RTD_CH0.rtd_res_raw * 400 / 32768;
    }
    // calculate RTD temperature (simple calc, +/- 2 deg C from -100C to 100C)
    m_currentTemp = ((double)RTD_CH0.rtd_res_raw / 32) - 256;
  }  // end of no-fault handling
  else
  {
    error = true;
    if (0x80 & RTD_CH0.status)
    {
      msg = "RTD High Threshold Met";  // RTD high threshold fault
    }
    else if (0x40 & RTD_CH0.status)
    {
      msg = "RTD Low Threshold Met";   // RTD low threshold fault
    }
    else if (0x20 & RTD_CH0.status)
    {
      msg = "REFin- > 0.85 x Vbias";   // REFin- > 0.85 x Vbias
    }
    else if (0x10 & RTD_CH0.status)
    {
      msg = "FORCE- open";             // REFin- < 0.85 x Vbias, FORCE- open
    }
    else if (0x08 & RTD_CH0.status)
    {
      msg = "FORCE- open";             // RTDin- < 0.85 x Vbias, FORCE- open
    }
    else if (0x04 & RTD_CH0.status)
    {
      msg = "Over/Under voltage fault";  // overvoltage/undervoltage fault
    }
    else
    {
      msg = "Unknown fault, check connection"; // print RTD temperature heading
    }
  }  // end of fault handling
  if (Serial.available() && error) {
    Serial.write(msg);
  }
  return m_currentTemp;
}

