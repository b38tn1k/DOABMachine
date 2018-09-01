#include "Arduino.h"
#include "Instrument.h"
#include "MMInterface.h"

void Instrument::setReverbLevel(uint8_t chan, uint8_t _reverbLevel){
  reverbLevel = _reverbLevel;
  midiSetChannelReverb(chan, reverbDecay, reverbLevel);
}

void Instrument::setReverbDecay(uint8_t chan, uint8_t _reverbDecay){
  reverbDecay = _reverbDecay;
  midiSetChannelReverb(chan, reverbDecay, reverbLevel);
}

void Instrument::setVolume(uint8_t chan, uint8_t _volume){
  volume = _volume;
  midiSetChannelVolume(chan, volume);
}

void Instrument::setInstrument(uint8_t chan, uint8_t _instrument){
  instrument = _instrument;
  midiSetInstrument(chan, instrument);
}

void Instrument::setBank(uint8_t chan, uint8_t _bank){
  bank = _bank;
  midiSetChannelBank(chan, bank);
}

void Instrument::playNote(uint8_t chan, uint8_t note) {
  midiNoteOn(chan, note, velocity);
}

void Instrument::stopNote(uint8_t chan, uint8_t note) {
  midiNoteOff(chan, note, velocity);
}

void Instrument::writeDefaults() {
  midiSetChannelBank(channel, bank);
  midiSetInstrument(channel, 0);
  midiSetChannelVolume(channel, 127);
  midiSetChannelReverb(channel, 0, 0);
}

