#ifndef EventSequence_h
#define EventSequence_h

#define POLY 8

#include "Instrument.h"

// a two bank sequencer with variable loop length,
// can store <POLY> number of notes at each event
// TODO:  REVERSE MODE | EDOM ESREVER

class EventSequence : public Instrument{ // could I just inherit timer also?
  private:
    // sequence step
    typedef struct stepEvent{
      uint8_t sequenceNumber;
      uint8_t notes[2][POLY] = {{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}};
      stepEvent *next;
      stepEvent *prev; // for reverse
    } stepEvent;
    bool forwards;
    uint8_t bank;
    void addNewStepToTail(uint8_t _sequenceNumber);
    void playAllNotesAtCurrentStep();
    void stopAllNotesAtCurrentStep();
  public:
    uint8_t sequence_length;
    EventSequence(uint8_t MIDIbank, uint8_t interface) : Instrument(MIDIbank, interface){bank = 0; forwards = true;}
    void step();
    void toggleBank();
    stepEvent *head, *tail, *current;
    void addNote2CurrentStep(uint8_t note);
};

#endif
