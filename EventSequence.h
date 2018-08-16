#ifndef EventSequence_h
#define EventSequence_h

#define POLY 5
#define BANK_COUNT 4

#include "Instrument.h"

// a two bank sequencer with variable loop length,
// can store <POLY> number of notes at each event
// TODO:  REVERSE MODE | EDOM ESREVER

class EventSequence : public Instrument{ // could I just inherit timer also?
  private:
    // sequence step
    typedef struct stepEvent{
      uint8_t sequenceNumber;
      uint8_t notes[BANK_COUNT][POLY] = {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}};
      stepEvent *next;
      stepEvent *prev; // for reverse
    } stepEvent;
    uint8_t futureNotes[POLY] = {0, 0, 0, 0, 0};
    bool forwards;
    uint8_t bank;
    void addNewStepToTail(uint8_t _sequenceNumber);
    void playAllNotesAtCurrentStep();
    void stopAllNotesAtCurrentStep();
  public:
    uint8_t sequence_length;
    EventSequence(uint8_t MIDIbank) : Instrument(MIDIbank){bank = 0; forwards = true;}
    void step(bool play);
    void toggleBank();
    stepEvent *head, *tail, *current;
    void addNote2CurrentStep(uint8_t note);
    void addNote2NextStep(uint8_t note);
    uint8_t nextStepMute[POLY] = {0, 0, 0, 0, 0};
    void clearSeq();
};

#endif
