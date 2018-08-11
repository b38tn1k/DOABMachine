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
#define ENTER_BUTTON 50
#define BACK_BUTTON 48
#define F1_BUTTON 52
#define F2_BUTTON 53
#define F3_BUTTON 51
#define F4_BUTTON 44
#define REVERB_DECAY A2
#define REVERB_LEVEL A1
#define VOLUME A0
// Serial RX Pin for VS1053
#define VS1053_RX  2
SoftwareSerial vs1053Serial(0, VS1053_RX);
// Interface
Button pads[5];
Button interfaceButtons[6];
uint8_t interfaceButtonPins[6] = {ENTER_BUTTON, BACK_BUTTON, F1_BUTTON, F2_BUTTON, F3_BUTTON, F4_BUTTON};

SerLCD lcd = SerLCD(5);
// Sequencer
EventSequence sequencer(VS1053_BANK_DRUMS1);  
Timer timer(440, NULL, NULL, CLK_OUT);
//Patch
typedef struct Patch {
  uint8_t pads[5] = {36, 35, 42, 70, 40};
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
  sequencer.sequence_length = 64; // max = 128
  // Set up pads
  for (int i = 0; i < 5; i++) {
    pads[i].init(INPUT, i+9); // Pads are on pins 9 through 13
    pads[i].tag = patch.pads[i];
  }
  //set up the interface buttons
  for (int i = 0; i < 6; i++) {
    interfaceButtons[i].init(INPUT_PULLUP, interfaceButtonPins[i]); // Pads are on pins 9 through 13
    pads[i].tag = patch.pads[i];
  }
}

uint8_t map_pot(int pot, double maximum, bool flipped) {
  double value = (analogRead(pot) / 1023.00) * maximum;
  if (flipped == true) { // pots wired backwards cause I am a dummy
    value = maximum - value;
  }
  return value;
}

void loop() {
  //MENU and INTERFACE
  for (int i = 0; i<6; i++){
    interfaceButtons[i].checkState();
    if (interfaceButtons[i].triggered = true){
      sequencer.toggleBank();
      //do menu stuff here
      //TODO: figure out menu stuff
//      Serial.println(i);
    }
    
  }
  // REVERB AND VOLUME
  sequencer.setReverbDecay(map_pot(REVERB_DECAY, 127, true));
  sequencer.setReverbLevel(map_pot(REVERB_LEVEL, 127, true));
  sequencer.setVolume(map_pot(VOLUME, 127, true));
  // PADS
  for (int i = 0; i < 5; i++) {
    pads[i].checkState();
    if (pads[i].triggered == true) {
      sequencer.playNote(pads[i].tag);
      sequencer.addNote2CurrentStep(pads[i].tag);
      
    }
  }
  // SEQUENCING
  if (timer.tick() == true) {
    sequencer.step();
    Serial.println(sequencer.current->sequenceNumber);
    //metronome for testing
    if (sequencer.current->sequenceNumber % 16 == 0 || sequencer.current->sequenceNumber == 0) {
      sequencer.playNote(31);
    }
    if (sequencer.current->sequenceNumber % 4 == 0) {
      sequencer.playNote(33);
    }
  }
}
