#ifndef MMInterface_h
#define MMInterface_h

#include <SoftwareSerial.h>

//Music Maker Interface:
//Communicates with Music Maker via SoftwareSerial 


class MMInterface: public SoftwareSerial {
  public:
    MMInterface(uint8_t transmitPin) : SoftwareSerial(0, transmitPin) {}
    void midiSetInstrument(uint8_t chan, uint8_t inst);
    void midiSetChannelVolume(uint8_t chan, uint8_t vol);
    void midiSetChannelReverb(uint8_t chan, uint8_t decay, uint8_t level);
    void midiSetChannelBank(uint8_t chan, uint8_t bank);
    void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel);
    void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel);
};

#endif
