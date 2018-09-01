#ifndef Encoder_h
#define Encoder_h
#include "Button.h"

class Encoder{
  public:
    Encoder(int _clk, int _dt, int _sw) : _sw(_sw), clk(_clk), dt(_dt) {};
    void init();
    bool tickUp;
    bool tickDown;
    int getValue();
    void update();
    Button sw;
    volatile int counter =0, previous;
    int currentCounter = 0;
    int interruptPin, _sw, dt, clk;
  private:
    int offset = 4;
};

#endif
