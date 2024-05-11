###What is that?

An Arduino Nano controlled door for an animal pen in the garden (for example ducks).

###What makes it special compared to the door controls you can already buy?

The door is controlled according to sunrise and sunset. Each day therefore has its own closing times. 

The main purpose of this is to make life easier for the owners, as mechanical timers do not adjust according to the time of year - so the closing times have to be corrected manually on a regular basis.

###How does it work?

The basis for controlling the door is the DS3231 RTC module. The module has a temperature-corrected timer and a 4KB EEPROM. Once the timer has been programmed to “now” and the EEPROM has been written with the closing times, there are individual closing times for 365/366 days a year.

###How to programm the EEPROM?

To program the EEPROM, I have created the “Time calculation” worksheet. On this website you can create a table showing when the sun rises or sets at your location on each day of the year:
https://www.sunearthtools.com/de/solar/sunrise-sunset-calendar.php
Transfer the times to the spreadsheet and add an offset for dusk. For our ducks, I have also installed a lamp in the barn that should come on 10 minutes before the door closes.

###Bill of Materials:

+PCB

+Box (https://sintron-shop.de/produkte/bauelemente-gehaeuse-zubehoer/gehaeuse/2601/modulgehaeuse-abs-115-x-90-x-55-mm-mit-transparentem-deckel?c=11175)

+DS3231 RTC (https://www.az-delivery.de/products/ds3231-real-time-clock)

+Relais (https://www.az-delivery.de/products/kf-301-relais-modul-mit-low-level-trigger)

+1.3" OLED (https://www.az-delivery.de/products/1-3zoll-i2c-oled-display)

+Arduino Nano (https://www.az-delivery.de/products/nano-v3-0-pro?variant=39457767915616)

+4 Screws M2.5x20 and 4 Nuts M2.5

+4 Standoff's M2x10 and 8 Screws M2x6

+Some Jumperheaders and Sockets
