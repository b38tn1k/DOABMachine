#include "Arduino.h"
#include "Instrument.h"
#include "MMInterface.h"

void Instrument::setReverbLevel(uint8_t _reverbLevel){
  reverbLevel = _reverbLevel;
  midiSetChannelReverb(channel, reverbDecay, reverbLevel);
}

void Instrument::setReverbDecay(uint8_t _reverbDecay){
  reverbDecay = _reverbDecay;
  midiSetChannelReverb(channel, reverbDecay, reverbLevel);
}

void Instrument::setVolume(uint8_t _volume){
  volume = _volume;
  midiSetChannelVolume(channel, volume);
}

void Instrument::setInstrument(uint8_t _instrument){
  instrument = _instrument;
  midiSetInstrument(channel, instrument);
}

void Instrument::setBank(uint8_t _bank){
  bank = _bank;
  midiSetChannelBank(channel, bank);
}

void Instrument::setChannel(uint8_t _channel){
  channel = _channel;
  midiSetChannelBank(channel, bank);
}

void Instrument::playNote(uint8_t note) {
  midiNoteOn(channel, note, velocity);
}

void Instrument::stopNote(uint8_t note) {
  midiNoteOff(channel, note, velocity);
}

void Instrument::writeDefaults() {
  midiSetChannelBank(channel, bank);
  midiSetInstrument(channel, 0);
  midiSetChannelVolume(channel, 127);
  midiSetChannelReverb(channel, 0, 0);
}

