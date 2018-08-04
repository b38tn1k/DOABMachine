#include "Arduino.h"
#include "SerLCD.h"

void SerLCD::clear() {
  write(254);
  write(0x01);
  flush();
}

void SerLCD::off() {
  write(254);
  write(0x08);
  flush();
}

void SerLCD::on() {
  write(254);
  write(0x0C);
  flush();
}

void SerLCD::setSplash() {
  clear();
  write(254);
  write(128);
  write("  DOABMachine  ");
  write(254);
  write(192);
  write("  b38tn1k.com  ");
  write(124);
  write(10);
  flush();
}

void SerLCD::reset() {
  clear();
  write(254);
  write(128);
}

