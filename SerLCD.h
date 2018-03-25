#ifndef SerLCD_h
#define SerLCD_h

#include <SoftwareSerial.h>

class SerLCD : public SoftwareSerial {
  public:
    SerLCD(uint8_t transmitPin) : SoftwareSerial(0, transmitPin) {}
};

#endif
