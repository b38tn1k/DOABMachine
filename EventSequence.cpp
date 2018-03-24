#include "EventSequence.h"

void EventSequence::addStepEvent(uint8_t _sequence_number) {
  stepEvent *temp = new stepEvent;
  temp->sequence_number = _sequence_number;
  temp->noteEvents = NULL;
  temp->next = NULL;
  if (head == NULL) {
    head = temp;
    tail = temp;
    current = head;
  } else {
    tail->next = temp;
    tail = temp;
  }
  temp = NULL;
}

void EventSequence::updateCurrentStep() {
  if (current == NULL) {
    addStepEvent(1);
  } else {
    uint8_t i = current->sequence_number;
    if (i < sequence_length) {
      if (current->next == NULL) {
        addStepEvent(++i);
        current = tail;
      } else {
        current = current->next;
      }
    } else {
      current = head;  
    }
  }
}

void EventSequence::addNoteEvent2CurrentStep(uint8_t _note, uint8_t _velocity, uint8_t _channel, uint8_t _bank) {
  noteEvent *temp = new noteEvent;
  temp->note = _note;
  temp->velocity = _velocity;
  temp->channel = _channel;
  temp->bank = _bank;

  //UMMMMMMMM

}

