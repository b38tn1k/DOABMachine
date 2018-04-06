#include <SoftwareSerial.h>
#include "Instrument.h"
#include "EventSequence.h"
#include "SerLCD.h"
#include "Timer.h"

#include "MMInterface.h"

// Patch names for every drum sound, not particularily space efficient but lives in PROGMEM
// need to strip off the NULL space surrounding or make every string the same length?
const char patchNames[][21] PROGMEM = {"High Q","Slap","Scratch Push [EXC 7]","Scratch Pull [EXC 7]","Sticks","Square Click","Metronome Click","Metronome Bell","Acoustic Bass Drum","Bass Drum 1","Side Stick","Acoustic Snare","Hand Clap","Electric Snare","Low Floor Tom","Closed Hi-hat [EXC 1]","High Floor Tom","Pedal Hi-hat [EXC 1]","Low Tom","Open Hi-hat [EXC 1]","Low-Mid Tom","High Mid Tom","Crash Cymbal 1","High Tom","Ride Cymbal 1","Chinese Cymbal","Ride Bell","Tambourine","Splash Cymbal","Cowbell","Crash Cymbal 2","Vibra-slap","Ride Cymbal 2","High Bongo","Low Bongo","Mute Hi Conga","Open Hi Conga","Low Conga","High Timbale","Low Timbale","High Agogo","Low Agogo","Cabasa","Maracas","Short Whistle [EXC 2]","Long Whistle [EXC 2]","Short Guiro [EXC 3]","Long Guiro [EXC 3]","Claves","Hi Wood Block","Low Wood Block","Mute Cuica [EXC 4]","Open Cuica [EXC 4]","Mute Triangle [EXC 5]","Open Triangle [EXC 5]","Shaker","Jingle bell","Bell tree","Castanets","Mute Surdo [EXC 6]","Open Surdo [EXC 6]"};

#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79
#define CLK_OUT 8

// Serial RX Pin for VS1053
#define VS1053_RX  2

SoftwareSerial myserial(0, 2);

EventSequence sequencer(VS1053_BANK_DRUMS1);  
Timer timer(480, NULL, NULL, CLK_OUT);

void setup() {
  pinMode(8, OUTPUT);
  Serial.begin(9600);
  myserial.begin(31250);
  sequencer.attachInterface(& myserial);
  sequencer.writeDefaults();
  sequencer.setInstrument(VS1053_BANK_DRUMS1);
  sequencer.sequence_length = 64;
}

void loop() {
  if (timer.tick() == true) {
    sequencer.step();
    
    //TEST!!
    if (sequencer.current->sequenceNumber == 0 || sequencer.current->sequenceNumber % 4 == 0) {
      sequencer.addNote2CurrentStep(36); //kik?
      sequencer.addNote2CurrentStep(35); //kik?
    }
    if (sequencer.current->sequenceNumber % 8 == 0) {
      sequencer.addNote2CurrentStep(40); //snare?
      sequencer.addNote2CurrentStep(32); //snare?
    }
    if (sequencer.current->sequenceNumber % 2 == 0) {
      sequencer.addNote2CurrentStep(42); //hat
      sequencer.addNote2CurrentStep(70); //hat
    }
    if (sequencer.current->sequenceNumber % 9 == 0) {
      sequencer.addNote2CurrentStep(76); //DJ
    }
    if (sequencer.current->sequenceNumber % 7 == 0) {
      sequencer.addNote2CurrentStep(77); //DJ
    }
    // END TEST
  }
}

/*
High Q
Slap
Scratch Push [EXC 7]
Scratch Pull [EXC 7]
Sticks
Square Click
Metronome Click
Metronome Bell
Acoustic Bass Drum
Bass Drum 1
Side Stick
Acoustic Snare
Hand Clap
Electric Snare
Low Floor Tom
Closed Hi-hat [EXC 1]
High Floor Tom
Pedal Hi-hat [EXC 1]
Low Tom
Open Hi-hat [EXC 1]
Low-Mid Tom
High Mid Tom
Crash Cymbal 1
High Tom
Ride Cymbal 1
Chinese Cymbal
Ride Bell
Tambourine
Splash Cymbal
Cowbell
Crash Cymbal 2
Vibra-slap
Ride Cymbal 2
High Bongo
Low Bongo
Mute Hi Conga
Open Hi Conga
Low Conga
High Timbale
Low Timbale
High Agogo
Low Agogo
Cabasa
Maracas
Short Whistle [EXC 2]
Long Whistle [EXC 2]
Short Guiro [EXC 3]
Long Guiro [EXC 3]
Claves
Hi Wood Block
Low Wood Block
Mute Cuica [EXC 4]
Open Cuica [EXC 4]
Mute Triangle [EXC 5]
Open Triangle [EXC 5]
Shaker
Jingle bell
Bell tree
Castanets
Mute Surdo [EXC 6]
Open Surdo [EXC 6]
*/
