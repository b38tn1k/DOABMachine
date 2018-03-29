#ifndef Instrument_h
#define Instrument_h

#include "MMInterface.h"

//Instrument:
//Set Instrument Channel characteristics:
//Reverb Level
//Reverb Decay
//Pass through notes to Sequence Event
//Play notes as the arrive from UI

class Instrument : public MMInterface {
  private:
    uint8_t channel;
    uint8_t instrument;
    uint8_t bank;
    uint8_t reverbLevel;
    uint8_t reverbDecay;
    uint8_t volume;
    uint8_t velocity;
  public:
    Instrument(uint8_t _bank) : bank(_bank) {velocity = 100; channel = 0; reverbLevel = 10; reverbDecay = 10;}
    void setReverbLevel(uint8_t reverbLevel);
    void setReverbDecay(uint8_t reverbDecay);
    void setVolume(uint8_t volume);
    void setChannel(uint8_t channel);
    void setBank(uint8_t bank);
    void setInstrument(uint8_t instrument);
    void playNote(uint8_t note);
    void stopNote(uint8_t note);
    void writeDefaults();
};

#endif
