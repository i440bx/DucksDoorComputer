#include "Arduino.h"
#include "Wire.h"
#include "uEEPROMLib.h"

uEEPROMLib eeprom(0x57);

unsigned int pos;

void setup() {
delay (2000);
	Serial.begin(9600);
  Serial.println("Serial OK");
	pos = 0;
}

void loop() {
	Serial.print(pos);
  Serial.print(": 0x");
	Serial.print(eeprom.eeprom_read(pos), HEX);
	Serial.println();

	pos++;
	delay(250);
}