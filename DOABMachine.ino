#include "Instrument.h"
#include "EventSequence.h"
#include "SerLCD.h"
#include "Timer.h"

#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79
// Serial RX Pin for VS1053
#define VS1053_RX  2

Instrument instrument(VS1053_BANK_DRUMS1, VS1053_RX);
EventSequence sequencer(VS1053_RX);
uint8_t sequence_length = 16;

void setup() {
  Serial.begin(9600);
  sequencer.sequence_length = 16;
}

void loop() {
  delay(500);
  sequencer.updateCurrentStep();
  Serial.print(sequencer.current->sequence_number);
  if (sequencer.current->sequence_number == 6 ) {
    sequencer.addNoteEvent2CurrentStep(1, 1, 1, 1);
  }
  Serial.println();
}