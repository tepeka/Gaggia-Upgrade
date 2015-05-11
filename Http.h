#ifndef Http_h
#define Http_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Http {
  public:
    Http();
    ~Http();
    void Get(String url, String params[]);
    
  private:
};

#endif
