#include "EventSequence.h"

void EventSequence::addStepEvent(uint8_t _sequence_number) {
  stepEvent *temp = new stepEvent;
  temp->sequence_number = _sequence_number;
  temp->next = NULL;
  if (head == NULL) {
    temp->prev = NULL;
    head = temp;
    tail = temp;
    current = head;
  } else {
    tail->next = temp;
    temp->prev = tail;
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
  int i;
  for (i = 0; i < POLY; i++) {
    if (current->note[i] == NULL) {
      current->note[i] = _note;
      current->velocity[i] = _velocity;
      current->channel[i] = _channel;
      current->bank[i] = _bank;
      break;
    }
  }
}

