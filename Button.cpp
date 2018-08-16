#include "Arduino.h"
#include "Button.h"

void Button::init(int mode, uint8_t _pin){
  pin = _pin;
  if (mode == INPUT_PULLUP){
    defaultState = HIGH;
  } else {
    defaultState = LOW;
  }
  state = defaultState;
  pinMode(pin, mode);
}

void Button::checkState() {
  uint8_t _state = digitalRead(pin);
  triggered = false;
  if (_state != state && state == defaultState) {
    triggered = true;
  }
  state = _state;
}

bool Button::isDown() {
  uint8_t _state = digitalRead(pin);
  if (_state == defaultState) {
    return true;
  }
  return false;
}

