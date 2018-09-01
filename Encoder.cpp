#include "Arduino.h"
#include "Encoder.h"
#include "Button.h"
//https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/
void Encoder::init() {
  sw.init(INPUT_PULLUP, _sw);
}

