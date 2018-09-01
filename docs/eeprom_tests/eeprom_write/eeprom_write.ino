
#include <EEPROM.h>

int saveLengths[5] = {5, 5, 4, 2, 2};
//char toSave[][5] = {"Hello", "World", "This", "Is", "me"};

int address = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
//  for (int i = 0; i<5; i++) {
//    for (int j = 0; j < saveLengths[i]; j++) {
//      Serial.print(toSave[i][j]); 
//      EEPROM.write(address, toSave[i][j]);
//      address++;
//    }
//  }
  address = 0; // reset

  for (int i = 0; i < 18; i++) {
    Serial.print(char(EEPROM.read(i)));
  }
  Serial.println();
  
  if (address == EEPROM.length()) {
    address = 0;
  }
}
