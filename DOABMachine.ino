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
const char drumNames[][21] PROGMEM = {"High Q","Slap","Scratch Push [EXC 7]","Scratch Pull [EXC 7]","Sticks","Square Click","Metronome Click","Metronome Bell","Acoustic Bass Drum","Bass Drum 1","Side Stick","Acoustic Snare","Hand Clap","Electric Snare","Low Floor Tom","Closed Hi-hat [EXC 1]","High Floor Tom","Pedal Hi-hat [EXC 1]","Low Tom","Open Hi-hat [EXC 1]","Low-Mid Tom","High Mid Tom","Crash Cymbal 1","High Tom","Ride Cymbal 1","Chinese Cymbal","Ride Bell","Tambourine","Splash Cymbal","Cowbell","Crash Cymbal 2","Vibra-slap","Ride Cymbal 2","High Bongo","Low Bongo","Mute Hi Conga","Open Hi Conga","Low Conga","High Timbale","Low Timbale","High Agogo","Low Agogo","Cabasa","Maracas","Short Whistle [EXC 2]","Long Whistle [EXC 2]","Short Guiro [EXC 3]","Long Guiro [EXC 3]","Claves","Hi Wood Block","Low Wood Block","Mute Cuica [EXC 4]","Open Cuica [EXC 4]","Mute Triangle [EXC 5]","Open Triangle [EXC 5]","Shaker","Jingle bell","Bell tree","Castanets","Mute Surdo [EXC 6]","Open Surdo [EXC 6]"};
const char patchNames[][127] PROGMEM = {"Acoustic Grand Piano","Bright Acoustic Piano","Electric Grand Piano","Honky-tonk Piano","Electric Piano 1","Electric Piano 2","Harpsichord","Clavi","Celesta","Glockenspiel","Music Box","Vibraphone","Marimba","Xylophone","Tubular Bells","Dulcimer","Drawbar Organ","Percussive Organ","Rock Organ","Church Organ","Reed Organ","Accordion","Harmonica","Tango Accordion","Acoustic Guitar (nylon)","Acoustic Guitar (steel)","Electric Guitar (jazz)","Electric Guitar (clean)","Electric Guitar (muted)","Overdriven Guitar","Distortion Guitar","Guitar Harmonics","Acoustic Bass","Electric Bass (finger)","Electric Bass (pick)","Fretless Bass","Slap Bass 1","Slap Bass 2","Synth Bass 1","Synth Bass 2","Violin","Viola","Cello","Contrabass","Tremolo Strings","Pizzicato Strings","Orchestral Harp","Timpani","String Ensembles 1","String Ensembles 2","Synth Strings 1","Synth Strings 2","Choir Aahs","Voice Oohs","Synth Voice","Orchestra Hit","Trumpet","Trombone","Tuba","Muted Trumpet","French Horn","Brass Section","Synth Brass 1","Synth Brass 2","Soprano Sax","Alto Sax","Tenor Sax","Baritone Sax","Oboe","English Horn","Bassoon","Clarinet","Piccolo","Flute","Recorder","Pan Flute","Blown Bottle","Shakuhachi","Whistle","Ocarina","Square Lead (Lead 1)","Saw Lead (Lead)","Calliope Lead (Lead 3)","Chiff Lead (Lead 4)","Charang Lead (Lead 5)","Voice Lead (Lead 6)","Fifths Lead (Lead 7)","Bass + Lead (Lead 8)","New Age (Pad 1)","Warm Pad (Pad 2)","Polysynth (Pad 3)","Choir (Pad 4)","Bowed (Pad 5)","Metallic (Pad 6)","Halo (Pad 7)","Sweep (Pad 8)","Rain (FX 1)","Sound Track (FX 2)","Crystal (FX 3)","Atmosphere (FX 4)","Brightness (FX 5)","Goblins (FX 6)","Echoes (FX 7)","Sci-fi (FX 8)","Sitar","Banjo","Shamisen","Koto","Kalimba","Bag Pipe","Fiddle","Shanai","Tinkle Bell","Agogo","Pitched Percussion","Woodblock","Taiko Drum","Melodic Tom","Synth Drum","Reverse Cymbal","Guitar Fret Noise","Breath Noise","Seashore","Bird Tweet","Telephone Ring","Helicopter","Applause","Gunshot"};
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
uint8_t channel = 0;

// Objects
Button pads[5];
Button interfaceButtons[6];
uint8_t interfaceButtonPins[6] = {ENTER_BUTTON, BACK_BUTTON, F1_BUTTON, F2_BUTTON, F3_BUTTON, F4_BUTTON};
Encoder f1Encoder(19, 24, 25); // check these pins, check the encoders, ehhhhh
Encoder f2Encoder(18, 28, 29);
Encoder menuEncoder(21, 22, 23);


serLCD lcd = serLCD(5);
EventSequence sequencer = EventSequence();
Timer timer(bpm, NULL, NULL, CLK_OUT);
SoftwareSerial vs1053Serial(0, VS1053_RX);
typedef struct Patch {
  uint8_t pads[4][5] = {{35, 38, 40, 43, 48}, {35, 38, 40, 43, 48}, {36, 35, 42, 70, 40},{36, 35, 42, 70, 40}};
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
  sequencer.midiSetChannelBank(3, VS1053_BANK_DRUMS1);
  sequencer.setInstrument(3, VS1053_BANK_DRUMS1);
  
  sequencer.midiSetChannelBank(2, VS1053_BANK_DRUMS2);
  sequencer.setInstrument(2, VS1053_BANK_DRUMS2);
  
  sequencer.midiSetChannelBank(1, VS1053_BANK_MELODY);
  sequencer.setInstrument(1, 92);
  
  sequencer.midiSetChannelBank(0, VS1053_BANK_MELODY);
  sequencer.setInstrument(0, 55);
  
  sequencer.sequence_length = 64; // max = 128, may change when Stop Notes required
  // Set up pads
  for (int i = 0; i < 5; i++) {
    pads[i].init(INPUT, i+9); // Pads are on pins 9 through 13
    pads[i].tag = patch.pads[channel][i];
  }
  //set up the interface buttons
  for (int i = 0; i < 6; i++) {
    interfaceButtons[i].init(INPUT_PULLUP, interfaceButtonPins[i]); // Pads are on pins 9 through 13
    pads[i].tag = patch.pads[channel][i];
    interfaceButtons[i].checkState();
  }
  // Encoders
  menuEncoder.init();
  f1Encoder.init();
  f2Encoder.init();
  attachInterrupt(digitalPinToInterrupt(menuEncoder.interruptPin), updateMenuEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(f1Encoder.interruptPin), updateF1Encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(f2Encoder.interruptPin), updateF2Encoder, CHANGE);
}

// these encoder interrupts probably effect the timer but isnt noticable (yet)
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

void updateF1Encoder() {
  int state = digitalRead(f1Encoder.clk);
  if (state != f1Encoder.previous) {
    if (digitalRead(f1Encoder.dt) != state) {
      f1Encoder.counter--;
    } else {
      f1Encoder.counter++;
    }
  }
  f1Encoder.previous = state;
}

void updateF2Encoder() {
  int state = digitalRead(f2Encoder.clk);
  if (state != f2Encoder.previous) {
    if (digitalRead(f2Encoder.dt) != state) {
      f2Encoder.counter--;
    } else {
      f2Encoder.counter++;
    }
  }
  f2Encoder.previous = state;
}



void loop() {
//  Serial.println(menuEncoder.counter);
  menuEncoder.update();
  if (menuEncoder.tickUp == true) {
    Serial.println("Up");
  }
  if (menuEncoder.tickDown == true) {
    Serial.println("Down");
  }
  
//  Serial.println(f1Encoder.counter);
//  Serial.println(f2Encoder.counter);
  //MENU and INTERFACE
  // Draw Menu & other Items to Scren
  lcd.setCursor(2, 1);
  lcd.print("BPM: ");
  lcd.print(bpm);
//  lcd.setCursor(2, 8);
  lcd.print(" CHAN: ");
  lcd.print(channel + 1);  //OBOE
  // Check all the buttons
  for (int i = 0; i<6; i++){
    interfaceButtons[i].checkState();
  }
  // Update Values
  // REVERB AND VOLUME POTS
  sequencer.setReverbDecay(channel, map_pot(REVERB_DECAY, 127, true));
  sequencer.setReverbLevel(channel, map_pot(REVERB_LEVEL, 127, true));
  sequencer.setVolume(channel, map_pot(VOLUME, 127, true));
  // Select Active Bank
  for (int i = 2; i < 6; i++) {
    if (interfaceButtons[i].triggered == true) {
      channel = i - 2;
      sequencer.channel = channel;
    }
  }
  
  // PADS
  for (int i = 0; i < 5; i++) {
    pads[i].checkState();
    if (pads[i].triggered == true) {
//      Serial.println(i);
      sequencer.playNote(channel, pads[i].tag);
      if (timer.tock() == false) {
        sequencer.addNote2CurrentStep(pads[i].tag);
      } else {
        sequencer.addNote2NextStep(pads[i].tag);
      }
    }
  }
  // if channel changes the pads will be updated
  for (int i = 0; i < 6; i++) {
    pads[i].tag = patch.pads[channel][i];
  }
  // SEQUENCING
  if (timer.tick() == true) {
    sequencer.step(true);
//    Serial.println(sequencer.current->sequenceNumber);
    //metronome for testing
    if (sequencer.current->sequenceNumber % 16 == 0 || sequencer.current->sequenceNumber == 0) {
      sequencer.playNote(3, 31);
    }
    if (sequencer.current->sequenceNumber % 4 == 0) {
      sequencer.playNote(3, 33);
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
