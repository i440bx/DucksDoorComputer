// Arduino Nano, OldBootlader, Pin A4 SDA & Pin A5 SCK(SCL)

#include <Wire.h>             //lib für I2C Kommunikation
#include <Adafruit_SH1106.h>  //lib fürs Display
#include <RTClib.h>           //lib für das RTC Modul
#include <Arduino.h>          //lib
#include <uEEPROMLib.h>       //lib für das lesen/scheiben vom EEPROM
#define OLED_RESET 4          //keine Ahnung, aber ohne gehts nicht
#define ADR_OpenStunde 0      //some naice idea
#define ADR_OpenMinute 1
#define ADR_CloseStunde 2
#define ADR_CloseMinute 3
#define ADR_LampeStunde 4
#define ADR_LampeMinute 5

RTC_DS3231 rtc;                       //"rtc" definieren
uEEPROMLib eeprom(0x57);              //Adresse des EEPROM definieren
Adafruit_SH1106 display(OLED_RESET);  //Display, keine Ahnung wofür aber ohne gehts nicht
const int DoorPin = 14;               //Pin für die Steuerung der Tür auf A0/Pin 14 legen
const int TasterTuer = 2;             //Pin für den TürToggler auf D2/ Pin 2 legen
const int TasterDisplay = 3;          //Pin für den DisplayKnopf auf D3/Pin 3 legen
const int TasterLED = 7;              //Pin für den DisplayKnopf auf D3/Pin 3 legen
const int LEDPin = 9 ;                //Pin für den DisplayKnopf auf D9/Pin 9 legen
int UhrZeitZahl;                      //ein paar Variablen initieren
int OpenZeitZahl;
int CloseZeitZahl;
int LampeZeitZahl;
int adresse;
int OpenStunde;
int OpenMinute;
int CloseStunde;
int CloseMinute;
int LampeStunde;
int LampeMinute;
int DisplayTimer;
int LEDTimer;
int LEDNachleuchtenTimer;
bool DoorBool;
bool OverwriteDoor;
bool ForceDoorAuf;
bool ForceDoorZu;
bool LEDNominalAn;
bool DisplayAn;
bool LEDAn;
bool LEDNachleuchten;

void setup() {

  pinMode(DoorPin, OUTPUT);              //Pinmodus festlegen
  pinMode(TasterTuer, INPUT_PULLUP);     //Pinmodus festlegen
  pinMode(TasterDisplay, INPUT_PULLUP);  //Pinmodus festlegen
  pinMode(TasterLED, INPUT_PULLUP);      //Pinmodus festlegen
  pinMode(LEDPin, OUTPUT);               //Pinmodus festlegen
  analogWrite(LEDPin, 0);                //die Lampe ausschalten
  pinMode(LED_BUILTIN, OUTPUT);          //Interne LED ansteuern

  display.begin(SH1106_SWITCHCAPVCC, 0x3C);  //something for display again
  display.setTextSize(1);                    //größe der Schrift
  display.setTextColor(WHITE);               //farbe der Schrift
  display.clearDisplay();                    //Display blank putzen
  display.display();                         //dem Display sagen die vorherigen Anweisungen durch zu führen

  //die untere IF-Schleife für den Fall das der RTC nicht gefunden wird
  if (!rtc.begin()) {
    display.setCursor(0, 0);
    display.println("Zeitgeber");
    display.setCursor(0, 8);
    display.println("nicht gefunden!");
    display.display();
    digitalWrite(LED_BUILTIN, HIGH);
    while (1) delay(1000);
  }

  //die untere IF-Schleife für den Fall das der RTC die Zeit vergessen hat
  if (rtc.lostPower()) {
    display.setCursor(0, 0);
    display.print("Batterie des");
    display.setCursor(0, 8);
    display.print("Zeitgebers leer!");
    display.display();
    digitalWrite(LED_BUILTIN, HIGH);
    while (1) delay(1000);
  }

  OverwriteDoor = 0;
  ForceDoorAuf = 0;
  ForceDoorZu = 0;
  DisplayTimer = 0;
  LEDTimer = 0;
  LEDNominalAn = 0;
  LEDNachleuchten = 0;
  LEDNachleuchtenTimer =0;
  DisplayAn = 1;
}


void loop() {
  DateTime now = rtc.now();  //jetzt holen

  adresse = (((now.month() - 1) * 31) + now.day()) * 10;        //Fancy calculation um jedem Tag im Jahr eine eindeutige Adresse im EEPROM zu zu weisen
  OpenStunde = eeprom.eeprom_read(adresse + ADR_OpenStunde);    //Die Zahl der Stunde zum öffnen ausm EEPROM ziehen und in die Variable legen
  OpenMinute = eeprom.eeprom_read(adresse + ADR_OpenMinute);    //wie oben nur als Minute
  CloseStunde = eeprom.eeprom_read(adresse + ADR_CloseStunde);  //wie oben nur als Stunde zum schließen
  CloseMinute = eeprom.eeprom_read(adresse + ADR_CloseMinute);  //wie oben nur als Minute
  LampeStunde = eeprom.eeprom_read(adresse + ADR_LampeStunde);
  LampeMinute = eeprom.eeprom_read(adresse + ADR_LampeMinute);

  UhrZeitZahl = (now.hour() * 100) + now.minute();    //Zahl der aktuellen Uhrzeit berechnen
  OpenZeitZahl = (OpenStunde * 100) + OpenMinute;     //Zahl der Öffnezeit berechnen
  CloseZeitZahl = (CloseStunde * 100) + CloseMinute;  //Zahl der Schließzeit berechnen
  LampeZeitZahl = (LampeStunde * 100) + LampeMinute;  //Zahl der Schließzeit berechnen

  //_____Soll-Status der Tür anhand der aktuellen Zeit und der Schließzeiten berechnen
  if ((UhrZeitZahl > OpenZeitZahl) && (UhrZeitZahl < CloseZeitZahl)) {  //Wenn die aktuelle Uhrzeit größer als die Öffne- und kleiner als die Schließzeit ist...
    DoorBool = 1;                                                       //...DoorBool 1 = Tür auf
  } else {
    DoorBool = 0;  //...DoorBool 0 = Tür zu
  }

  //_____Soll-Status der Lampe anhand der aktuellen Zeit und der Lampenzeiten berechnen
  if ((UhrZeitZahl >= LampeZeitZahl) && (UhrZeitZahl <= CloseZeitZahl)) {  //Wenn die aktuelle Uhrzeit gleich der Lampenzeit ist...
    LEDNominalAn = 1;                        //...LED nach Zeit einschalten
  }

  //_____Displaytaster abfragen
  if (digitalRead(TasterDisplay) == 0) {
    DisplayAn = 1;  //Wenn gedrückt die Bool aktiv setzen
  }

  //_____LED-taster abfragen
  if (digitalRead(TasterLED) == 0) {
    LEDAn = 1;      //Wenn gedrückt die Bool active setzen
    DisplayAn = 1;  //und für den User das Display einschalten
    LEDTimer = 0;  //den Timer für die LEDs zurücksetzen
  }

  //_____Türtaster abfragen
  if (digitalRead(TasterTuer) == 0) {
    OverwriteDoor = 1;  //Das Überschreiben der reguläten Schließzeiten aktivieren
    DisplayAn = 1;      //und für den User das Display einschalten
  }

  //_____Wenn die Tür zu sein soll, aber der Taster gedrückt wurde...
  if ((DoorBool == 0) && (OverwriteDoor == 1)) {
    ForceDoorAuf = 1;  //...wie der Name der variablen sagt
  }

  //_____Wenn die Tür auf sein soll, aber der Taster gedrückt wurde...
  if ((DoorBool == 1) && (OverwriteDoor == 1)) {
    ForceDoorZu = 1;  //...wie der Name der variablen sagt
  }

  //_____Wenn die Uhrzeit = regulären Schließ- oder Öffnezeit ist das Überschreiben der Tür deaktivieren
  if ((UhrZeitZahl == CloseZeitZahl) || (UhrZeitZahl == OpenZeitZahl)) {
    OverwriteDoor = 0;
    ForceDoorZu = 0;
    ForceDoorAuf = 0;
  }

  //_____Wenn die Uhrzeit = regulären Schließzeit ist das Faden der Lampe aktivieren
  if ((UhrZeitZahl == CloseZeitZahl) ) {
    LEDNominalAn = 0;
    LEDAn = 1;
  }

  //_____Falls die Tür vorzeitig geschlossen oder geöffnet werden soll...
  if ((ForceDoorZu == 1) || (ForceDoorAuf == 1)) {
    DoorBool = !DoorBool;  //...die DoorBool invertieren
  }

  digitalWrite(DoorPin, !DoorBool);  //die invertierte DoorBoolean schreiben, da der Ausgang active Low ist

  //_____Wenn das Display 60sec an war, die DisplayBoolean und den Timer zurück setzten
  if (DisplayTimer >= 60) {
    display.clearDisplay();
    display.display();
    DisplayAn = 0;
    DisplayTimer = 0;
  }

  //_____Wenn die LED 2 Minuten an war, die LED-Boolean, den Timer und den Pin zurück setzten
  if (LEDTimer == 124) {
    LEDAn = 0;
    LEDTimer = 0;
    analogWrite(LEDPin, 4);
    LEDNachleuchten = 1;
  }

  //_____Falls der LED-Knopf gedrückt wurde die LED für 2 Minuten einschalten
  if (LEDAn == 1) {
    analogWrite(LEDPin, 128 - LEDTimer);  //die LED auf 50% einschalten
    LEDTimer = LEDTimer + 1;              //den LED-Timer um 1 erhöhen
  }

    //_____Falls die LED nach der Zeit eingschaltet werden soll...
  if (LEDNominalAn == 1) {
    analogWrite(LEDPin, 128);  //die LED auf 50% einschalten
  }

  //_____Wenn die LED 2 Minuten an war, die LED-Boolean, den Timer und den Pin zurück setzten
  if (LEDNachleuchten == 1) {
    LEDNachleuchtenTimer = LEDNachleuchtenTimer + 1;
  }

  if (LEDNachleuchtenTimer == 180) {
    analogWrite(LEDPin, 3);
  } else if (LEDNachleuchtenTimer == 181) {
    analogWrite(LEDPin, 2);
  } else if (LEDNachleuchtenTimer == 182) {
    analogWrite(LEDPin, 1);
  } else if (LEDNachleuchtenTimer == 183) {
    analogWrite(LEDPin, 0);
    LEDNachleuchten = 0; 
    LEDNachleuchtenTimer = 0;
  }

  //_____Falls der Displayknopf gedrückt wurde allerei Kram aufs Display schreiben
  if (DisplayAn == 1) {
    display.clearDisplay();

    display.setCursor(0, 0);              //oben links im Display
    char buf1[] = "DDD DD.MM.YYYY";       //Jetzt als lesbaren Datumstext ummodeln
    display.println(now.toString(buf1));  //und aufs Display legen

    display.setCursor(0, 8);            //zweite Reihe im Display
    char buf2[] = "hh:mm:ss";           //Jetzt als lesbaren Uhrzeittext ummodeln
    display.print(now.toString(buf2));  //und aufs Display legen
    display.print(" Sommerzeit");       //mit der Info das der RTC immer auf Sommerzeit läuft

    char buf[22];
    display.setCursor(0, 16);
    snprintf(buf, sizeof(buf), "Auf ab:  %02d:%02d Uhr", OpenStunde, OpenMinute);
    display.print(buf);  // Öffnezeit anzeigen

    display.setCursor(0, 24);
    snprintf(buf, sizeof(buf), "Zu ab:   %02d:%02d Uhr", CloseStunde, CloseMinute);
    display.print(buf);  // Schließzeit anzeigen

    if (OverwriteDoor == 1) {
      display.setCursor(0, 32);
      display.print("Tur vorzeitig bewegt");
      display.drawPixel(7, 32, WHITE);
      display.drawPixel(9, 32, WHITE);
    }

    display.setCursor(0, 40);
    if (DoorBool == 1) {
      display.print("Relais an / Tur auf");
      display.drawPixel(79, 40, WHITE);
      display.drawPixel(81, 40, WHITE);
    }

    if (DoorBool == 0) {
      display.print("Relais aus / Tur zu");
      display.drawPixel(85, 40, WHITE);
      display.drawPixel(87, 40, WHITE);
    }

    display.setCursor(0, 56);
    display.print("Temperatur: ");
    display.print(rtc.getTemperature());
    display.print("*C");  //Temperatur ausm RTC holen und aufs Display legen

    display.display();  //Jezt alles aufs Display

    DisplayTimer = DisplayTimer + 1;  //den Displaytimer um 1 erhöhen
  }

  delay(1000);  //1 Sekunde Warten
}
