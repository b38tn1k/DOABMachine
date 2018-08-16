#include <SoftwareSerial.h>
#include "Instrument.h"
#include "EventSequence.h"
#include "SerLCD.h"
#include "Timer.h"
#include "Button.h"
#include "MMInterface.h"

#define MENU_SIZE 13
#define EDIT_PATCH 0
#define LOAD_PATCH 1
#define SAVE_PATCH 2
#define SYNC_MODE 3
#define BPM 4
#define PLAYPAUSE 5
#define OVERDUB 6
#define REVERSE 7
#define SEQLENGTH 8
#define SEQRES 9
#define METRO 10
#define SWING 11
#define SEQ_CLEAR 12
char menu[][MENU_SIZE] = {"EDIT PATCH", "LOAD PATCH", "SAVE PATCH", "SYNC_MODE", "BPM", "PLAY/PAUSE", "OVERDUB", "REVERSE", "SEQ LENGTH", "SEQ RES", "METRO", "SWING", "SEQ CLEAR"};

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
#define MENU_POT A5

// Serial RX Pin for VS1053
#define VS1053_RX  2
SoftwareSerial vs1053Serial(0, VS1053_RX);
// Interface
Button pads[5];
Button interfaceButtons[6];
#define OK 0
#define BACK 1
#define F1 2
#define F2 3
#define F3 4
#define F4 5
uint8_t interfaceButtonPins[6] = {ENTER_BUTTON, BACK_BUTTON, F1_BUTTON, F2_BUTTON, F3_BUTTON, F4_BUTTON};
int cursor;
int cursor_latch;

serLCD lcd = serLCD(5);
// Sequencer
EventSequence sequencer(VS1053_BANK_DRUMS1);
int bpm = 440;
Timer timer(bpm, NULL, NULL, CLK_OUT);
//Patch
typedef struct Patch {
  uint8_t pads[5] = {36, 35, 42, 70, 40};
  uint8_t volumes[127];
} Patch;
Patch patch;

void setup() {
  Serial.begin(9600);
//  Serial.println("Hello, I have reset");
  vs1053Serial.begin(31250);
  lcd.clear();
  lcd.home();
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
    interfaceButtons[i].checkState();
  }
}



void loop() {
  //MENU and INTERFACE
  // Draw Menu & other Items to Scren
  if (interfaceButtons[OK].isDown() == false) {
    cursor = map_pot(MENU_POT, MENU_SIZE-1, true);
  }
  if (cursor != cursor_latch) {
    lcd.clear();
    lcd.home();
    lcd.print(menu[cursor]);
    cursor_latch = cursor;
  }
  lcd.setCursor(2, 1);
  lcd.print("BPM: ");
  lcd.print(bpm);
  // Check all the buttons
  for (int i = 0; i<6; i++){
    interfaceButtons[i].checkState();
  }
  // Act on Button Inputs. I tried seperating this but was too complicated.. this looks ugly but works
  if (interfaceButtons[OK].triggered == true) {
    if (cursor == SEQ_CLEAR) {
      sequencer.clearSeq();
    }
  }
  if (interfaceButtons[OK].isDown() == true) {
    if (cursor_latch = BPM) {
      bpm = map_pot(MENU_POT, 800, true);
      if (bpm < 400) {bpm = 400;}
    }
  }
  // Update Values
  timer.setBPM(bpm);
  // REVERB AND VOLUME POTS
  sequencer.setReverbDecay(map_pot(REVERB_DECAY, 127, true));
  sequencer.setReverbLevel(map_pot(REVERB_LEVEL, 127, true));
  sequencer.setVolume(map_pot(VOLUME, 127, true));
  // PADS
  for (int i = 0; i < 5; i++) {
    pads[i].checkState();
    if (pads[i].triggered == true) {
      sequencer.playNote(pads[i].tag);
      if (timer.tock() == false) {
        sequencer.addNote2CurrentStep(pads[i].tag);
      } else {
        sequencer.addNote2NextStep(pads[i].tag);
      }
    }
  }
  // SEQUENCING
  if (timer.tick() == true) {
    sequencer.step(true);
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

int map_pot(int pot, double maximum, bool flipped) {
  double value = (analogRead(pot) / 1023.00) * maximum;
  if (flipped == true) { // pots wired backwards cause I am a dummy
    value = maximum - value;
  }
  return value;
}
