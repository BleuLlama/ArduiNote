# ArduiNote
An ATMega 328 Arduino-based slide presentation tool (Arduino + Keynote)

More info to come here in the future, but for now here are the basics:

##Software:

1. Copy the "ArduiNote" folder into your Arduino Sketches folder
2. Install the TVOut library https://code.google.com/p/arduino-tvout/
3. Download it to your Uno or whatever

## Hardware:
- This requires an ATMega 328. It will not work on a '168.
- I use an Uno R3
- Hook up the video cable, as per the TVOut page to pins 8, 9 and ground
- Hook up two pushbuttons to Analog pins A0 and A1.  These are the Next/Previous slide buttons.
 1. connect a 10kohm (Brown Black Orange) resistor from +5 to A0 (pullup - HIGH when idle)
 3. connect one side of a pushbutton 1 to A0
 4. connect the other side of pushbutton 1 to GROUND
 5. connect a 10kohm (Brown Black Orange) resistor from +5 to A1 (pullup - HIGH when idle)
 6. connect one side of a pushbutton 2 to A1
 7. connect the other side of pushbutton 1 to GROUND

##Notes:
 - The TVOut page indicates that it runs on pins 7 and 9.  This is incorrect. It works on pins 8 and 9.
 - Everything is hardcoded into the sketch for now
 
---

##Future ideas:
 - Store slides on a SD card, eg:
  - SD/p/pres1/slide0.txt
  - SD/p/pres1/slide1.txt
 - add a bit to list the directories in SD/p and let the user pick one
 - Sound clicks when changing slides?
