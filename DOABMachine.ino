#include <SoftwareSerial.h>
#include "Instrument.h"
#include "EventSequence.h"
#include "SerLCD.h"
#include "Timer.h"
#include "Button.h"
#include "MMInterface.h"
#include "Encoder.h"


/*              
 *               / EDIT  - EDIT()
 *  PATCH       - LOAD  - LOAD()
 *              \ SAVE  - SAVE()
 *              
 *  SYNC        - INTERNAL / EXTERNAL
 *  
 *  BPM         - SET(60, 255)
 *  
 *  TRANSPORT   - PLAY / PAUSE
 *  
 *  OVERDUB     - ON / OFF
 *  
 *  REVERSE     - FORWARDS / REVERSE
 *  
 *  METRONOME   - ON / OFF
 *              \ TIME SIGNATURE - SET()
 *  
 *  SWING       - SET(-50%, 50%)
 *  
 *              / LENGTH  - SET(1, 64)
 *  SEQUENCE    - CLEAR
 *              \ RESOLUTION - SET(int * timeSigDenominator) 
 */

// Patch names for every drum sound
const char patchNames[][21] PROGMEM = {"High Q","Slap","Scratch Push [EXC 7]","Scratch Pull [EXC 7]","Sticks","Square Click","Metronome Click","Metronome Bell","Acoustic Bass Drum","Bass Drum 1","Side Stick","Acoustic Snare","Hand Clap","Electric Snare","Low Floor Tom","Closed Hi-hat [EXC 1]","High Floor Tom","Pedal Hi-hat [EXC 1]","Low Tom","Open Hi-hat [EXC 1]","Low-Mid Tom","High Mid Tom","Crash Cymbal 1","High Tom","Ride Cymbal 1","Chinese Cymbal","Ride Bell","Tambourine","Splash Cymbal","Cowbell","Crash Cymbal 2","Vibra-slap","Ride Cymbal 2","High Bongo","Low Bongo","Mute Hi Conga","Open Hi Conga","Low Conga","High Timbale","Low Timbale","High Agogo","Low Agogo","Cabasa","Maracas","Short Whistle [EXC 2]","Long Whistle [EXC 2]","Short Guiro [EXC 3]","Long Guiro [EXC 3]","Claves","Hi Wood Block","Low Wood Block","Mute Cuica [EXC 4]","Open Cuica [EXC 4]","Mute Triangle [EXC 5]","Open Triangle [EXC 5]","Shaker","Jingle bell","Bell tree","Castanets","Mute Surdo [EXC 6]","Open Surdo [EXC 6]"};
// Midi Message Constants
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// IO Assignment
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
#define VS1053_RX  2

// Interface
#define OK 0
#define BACK 1
#define F1 2
#define F2 3
#define F3 4
#define F4 5

// Variables
int bpm = 120;
uint8_t bank = 0;

// Objects
Button pads[5];
Button interfaceButtons[6];
uint8_t interfaceButtonPins[6] = {ENTER_BUTTON, BACK_BUTTON, F1_BUTTON, F2_BUTTON, F3_BUTTON, F4_BUTTON};
Encoder f1Encoder(22, 24, 26);
Encoder f2Encoder(28, 30, 32);
Encoder menuEncoder(21, 22, 23);


serLCD lcd = serLCD(5);
EventSequence sequencer(VS1053_BANK_DRUMS1);
Timer timer(bpm, NULL, NULL, CLK_OUT);
SoftwareSerial vs1053Serial(0, VS1053_RX);
typedef struct Patch {
  uint8_t pads[4][5] = {{36, 35, 42, 70, 40}, {36, 35, 42, 70, 40}, {36, 35, 42, 70, 40},{36, 35, 42, 70, 40}};
  uint8_t volumes[127];
} Patch;
Patch patch;

void setup() {
  // Serial comms
  Serial.begin(9600);
  Serial.println("Hello");
  vs1053Serial.begin(31250);
  lcd.clear();
  lcd.home();
  // Sequencer Setup
  sequencer.attachInterface(& vs1053Serial);
  sequencer.writeDefaults();
  sequencer.setInstrument(VS1053_BANK_DRUMS1);
  sequencer.sequence_length = 64; // max = 128, may change when Stop Notes required
  // Set up pads
  for (int i = 0; i < 5; i++) {
    pads[i].init(INPUT, i+9); // Pads are on pins 9 through 13
    pads[i].tag = patch.pads[bank][i];
  }
  //set up the interface buttons
  for (int i = 0; i < 6; i++) {
    interfaceButtons[i].init(INPUT_PULLUP, interfaceButtonPins[i]); // Pads are on pins 9 through 13
    pads[i].tag = patch.pads[bank][i];
    interfaceButtons[i].checkState();
  }
  // Encoders
  menuEncoder.init();
  attachInterrupt(digitalPinToInterrupt(menuEncoder.interruptPin), updateMenuEncoder, CHANGE);
//  f1Encoder.init();
//  f2Encoder.init();
}

void updateMenuEncoder() {
  int state = digitalRead(menuEncoder.clk);
  if (state != menuEncoder.previous) {
    if (digitalRead(menuEncoder.dt) != state) {
      menuEncoder.counter--;
    } else {
      menuEncoder.counter++;
    }
  }
  menuEncoder.previous = state;
}

void loop() {
  Serial.println(menuEncoder.counter);
  lcd.setCursor(1, 1);
  lcd.print(menuEncoder.counter);
//  f1Encoder.update();
//  f2Encoder.update();
  //MENU and INTERFACE
  // Draw Menu & other Items to Scren
  lcd.setCursor(2, 1);
  lcd.print("BPM: ");
  lcd.print(bpm);
//  lcd.setCursor(2, 8);
  lcd.print(" BANK: ");
  lcd.print(bank + 1);  //OBOE
  // Check all the buttons
  for (int i = 0; i<6; i++){
    interfaceButtons[i].checkState();
  }
  // Update Values
  // REVERB AND VOLUME POTS
  sequencer.setReverbDecay(map_pot(REVERB_DECAY, 127, true));
  sequencer.setReverbLevel(map_pot(REVERB_LEVEL, 127, true));
  sequencer.setVolume(map_pot(VOLUME, 127, true));
  // Select Active Bank
  for (int i = 2; i < 6; i++) {
    if (interfaceButtons[i].triggered == true) {
      bank = i - 2;
    }
  }
  // PADS
  for (int i = 0; i < 5; i++) {
    pads[i].checkState();
    if (pads[i].triggered == true) {
//      Serial.println(i);
      sequencer.playNote(pads[i].tag);
      if (timer.tock() == false) {
        sequencer.addNote2CurrentStep(pads[i].tag);
      } else {
        sequencer.addNote2NextStep(pads[i].tag);
      }
    }
  }
  // if bank changes the pads will be updated
  for (int i = 0; i < 6; i++) {
    pads[i].tag = patch.pads[bank][i];
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
