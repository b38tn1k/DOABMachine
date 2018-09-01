#include "Arduino.h"
#include "Encoder.h"
#include "Button.h"
void Encoder::init() {
  sw.init(INPUT_PULLUP, _sw);
  pinMode(dt, INPUT);
  pinMode(clk, INPUT);
  interruptPin = clk;
  previous = digitalRead(clk);
}
