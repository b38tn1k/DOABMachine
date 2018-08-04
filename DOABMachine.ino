#include <SoftwareSerial.h>
#include "Instrument.h"
#include "EventSequence.h"
#include "SerLCD.h"
#include "Timer.h"
#include "Button.h"
#include "MMInterface.h"

// Patch names for every drum sound
const char patchNames[][21] PROGMEM = {"High Q","Slap","Scratch Push [EXC 7]","Scratch Pull [EXC 7]","Sticks","Square Click","Metronome Click","Metronome Bell","Acoustic Bass Drum","Bass Drum 1","Side Stick","Acoustic Snare","Hand Clap","Electric Snare","Low Floor Tom","Closed Hi-hat [EXC 1]","High Floor Tom","Pedal Hi-hat [EXC 1]","Low Tom","Open Hi-hat [EXC 1]","Low-Mid Tom","High Mid Tom","Crash Cymbal 1","High Tom","Ride Cymbal 1","Chinese Cymbal","Ride Bell","Tambourine","Splash Cymbal","Cowbell","Crash Cymbal 2","Vibra-slap","Ride Cymbal 2","High Bongo","Low Bongo","Mute Hi Conga","Open Hi Conga","Low Conga","High Timbale","Low Timbale","High Agogo","Low Agogo","Cabasa","Maracas","Short Whistle [EXC 2]","Long Whistle [EXC 2]","Short Guiro [EXC 3]","Long Guiro [EXC 3]","Claves","Hi Wood Block","Low Wood Block","Mute Cuica [EXC 4]","Open Cuica [EXC 4]","Mute Triangle [EXC 5]","Open Triangle [EXC 5]","Shaker","Jingle bell","Bell tree","Castanets","Mute Surdo [EXC 6]","Open Surdo [EXC 6]"};
// Midi Message Constants
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79
#define CLK_OUT 8
// Serial RX Pin for VS1053
#define VS1053_RX  2
SoftwareSerial vs1053Serial(0, VS1053_RX);
// Interface
Button pads[5];
SerLCD lcd = SerLCD(5);
// Sequencer
EventSequence sequencer(VS1053_BANK_DRUMS1);  
Timer timer(480, NULL, NULL, CLK_OUT);
//Patch
typedef struct Patch {
  uint8_t pads[5] = {36, 35, 42, 70, 40};;
//  uint8_t volumes[127];
} Patch;
Patch patch;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, I have reset");
  vs1053Serial.begin(31250);
  lcd.begin(9600);
//  lcd.reset();
//  lcd.setSplash();
  lcd.flush();
  lcd.print("Hello, this     ");
  lcd.print("doesn't work yet");
  sequencer.attachInterface(& vs1053Serial);
  sequencer.writeDefaults();
  sequencer.setInstrument(VS1053_BANK_DRUMS1);
  sequencer.sequence_length = 64;
  // Set up pads
  for (int i = 0; i < 5; i++) {
    pads[i].init(INPUT, i+9); // Pads are on pins 9 through 13
    pads[i].tag = patch.pads[i];
  }
  
}

void loop() {
  for (int i = 0; i < 5; i++) {
    pads[i].checkState();
    if (pads[i].triggered == true) {
//      Serial.println(i);
      sequencer.playNote(pads[i].tag);
      sequencer.addNote2CurrentStep(pads[i].tag);
      
    }
  }
  if (timer.tick() == true) {
    sequencer.step();
//    Serial.println(sequencer.current->sequenceNumber);
    //metronome for testing
    if (sequencer.current->sequenceNumber % 16 == 0 || sequencer.current->sequenceNumber == 0) {
      sequencer.playNote(31);
    }
    if (sequencer.current->sequenceNumber % 4 == 0) {
      sequencer.playNote(33);
    }
  }
}
