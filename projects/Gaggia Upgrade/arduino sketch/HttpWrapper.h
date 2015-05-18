#ifndef HttpWrapper_h
#define HttpWrapper_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include <Ethernet.h>

class HttpWrapper {
  public:
    HttpWrapper();
    ~HttpWrapper();
    void Get(char server[], String path);
    
  private:
};

#endif
