#include "EventSequence.h"
#include "Arduino.h"


void EventSequence::addNewStepToTail(uint8_t _sequenceNumber) {
  stepEvent *temp = new stepEvent;
  temp->sequenceNumber = _sequenceNumber;
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

void EventSequence::step(bool play) {
  if (current == NULL) {
    addNewStepToTail(1);
  } else {
    stopAllNotesAtCurrentStep();
    uint8_t i = current->sequenceNumber;
    if (i < sequence_length) {
      if (current->next == NULL) {
        addNewStepToTail(++i);
        current = tail;
      } else {
        current = current->next;
      }
    } else {
      current = head;  
    }
  }
  if (play == true) {
    playAllNotesAtCurrentStep();
  }
  // add any early notes that have already played to the loop
  for (int i = 0; i < POLY; i++) {
    if (futureNotes[i] != 0) {
      addNote2CurrentStep(futureNotes[i]);
      futureNotes[i] = 0;
    }
  }
}

void EventSequence::clearSeq() {
  Serial.println("clear");
  for (int i = 0; i < sequence_length; i++) {
    step(false);
    for (int j = 0; j < POLY; j++) {
      for (int k = 0; k < CHANNEL_COUNT; k++) {
        current->notes[channel][j] = 0;
      }
    }
  }
}

void EventSequence::addNote2CurrentStep(uint8_t _note) {
  int i;
  for (i = 0; i < POLY; i++) {
    if (current->notes[channel][i] == _note) {
      break; // don't double record notes
    }
    if (current->notes[channel][i] == NULL) {
      current->notes[channel][i] = _note;
      break;
    }
  }
}

void EventSequence::addNote2NextStep(uint8_t _note) {
  for (int i = 0; i < POLY; i++) {
    if (futureNotes[i] == 0) {
      futureNotes[i] = _note;
      break;
    }
  }
}

void EventSequence::playAllNotesAtCurrentStep() {
  int i;
  for (int j = 0; j < CHANNEL_COUNT; j++) {
    for (i = 0; i < POLY; i++) {
      if (current->notes[j][i] == 0) {
        break;
      } else {
       playNote(j, current->notes[j][i]); 
      }
    }
  }
}

void EventSequence::stopAllNotesAtCurrentStep() {
  int i;
  for (int j = 0; j < CHANNEL_COUNT; j++) {
    for (i = 0; i < POLY; i++) {
      if (current->notes[j][i] == NULL) {
        break;
      } else {
       stopNote(j, current->notes[j][i]); 
      }
    }
  }
}
