#ifndef EventSequence_h
#define EventSequence_h

#include "MMInterface.h"

//Trigger sequence events, play and record. Stores all note events for Event within Sequence:
//Channel
//Bank
//Note
//Velocity
//Next step


class EventSequence : public MMInterface {
  private:
    // notes to be triggered at each sequence step
    typedef struct noteEvent{
      uint8_t note;
      uint8_t channel;
      uint8_t velocity;
      uint8_t bank;
      noteEvent *next;
    } noteEvent;
    // sequence step
    typedef struct stepEvent{
      uint8_t sequence_number;
      noteEvent *noteHead, *noteTail, *noteEvents;
      stepEvent *next;
    } stepEvent;
  public:
    EventSequence(uint8_t interface) : MMInterface(interface){}
    void addStepEvent(uint8_t _sequence_number);
    void updateCurrentStep();
    stepEvent *head, *tail, *current;
    uint8_t sequence_length;
    void addNoteEvent2CurrentStep(uint8_t note, uint8_t velocity, uint8_t channel, uint8_t bank);    
};

#endif
