#include "Arduino.h"
#include "MMInterface.h"

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_REVERB_LEVEL 0x05b
#define MIDI_CHAN_REVERB_DECAY 0x0c
#define MIDI_CHAN_PROGRAM 0xC0

void MMInterface::attachInterface(SoftwareSerial * interface) {
  myserial = interface;
}

void MMInterface::midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;
  
  myserial->write(MIDI_CHAN_PROGRAM | chan);  
  myserial->write(inst);
}


void MMInterface::midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;
  
  myserial->write(MIDI_CHAN_MSG | chan);
  myserial->write(MIDI_CHAN_VOLUME);
  myserial->write(vol);
}

void MMInterface::midiSetChannelReverb(uint8_t chan, uint8_t decay, uint8_t level) {
  if (chan > 15) return;
  if (level > 127) return;
  if (decay > 127) return;
  
  myserial->write(MIDI_CHAN_MSG | chan);
  myserial->write(MIDI_CHAN_REVERB_LEVEL);
  myserial->write(level);
  myserial->write(MIDI_CHAN_MSG | chan);
  myserial->write(MIDI_CHAN_REVERB_DECAY);
  myserial->write(decay);
}

void MMInterface::midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;
  
  myserial->write(MIDI_CHAN_MSG | chan);
  myserial->write((uint8_t)MIDI_CHAN_BANK);
  myserial->write(bank);
}

void MMInterface::midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  myserial->write(MIDI_NOTE_ON | chan);
  myserial->write(n);
  myserial->write(vel);
}

void MMInterface::midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;
  
  myserial->write(MIDI_NOTE_OFF | chan);
  myserial->write(n);
  myserial->write(vel);
}
