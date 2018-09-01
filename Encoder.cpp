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

void Encoder::update() {
  tickUp = false;
  tickDown = false;
  if (currentCounter > (counter + offset) or currentCounter < (counter - offset)) {
    if (currentCounter < counter) {
      tickUp = true;
      tickDown = false;
    } else if (currentCounter > counter) {
      tickDown = true;
      tickUp = false;
    }
    currentCounter = counter;
  }
}

