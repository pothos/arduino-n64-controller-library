# Arduino N64 Controller Library

Based on the work in http://www.instructables.com/id/Use-an-Arduino-with-an-N64-controller/ here comes a comfortable library for usage with e.g. Arduino Uno. For NES there is already http://code.google.com/p/nespad/ . This library uses inline assembly and controllers can be attached to PIN 0 up to 13. But be aware that it's not written in best way possible. Place the folder N64Controller into your folder 'libraries'.

I used it in combination with TVout ( http://code.google.com/p/arduino-tvout/ ) and EEPROM ( http://arduino.cc/playground/Code/EEPROMWriteAnything ) for highscore I modified an existing Tetris port which itself uses Simple Tetris Clone under MIT license to be using this library here and the result is quite nice: http://pothos.blogsport.eu/files/2012/03/N64Tetris.zip


## Example code for library usage

```cpp
#include <N64Controller.h>

N64Controller player1 (12); // this controller for player one is on PIN 12

void setup() {
    player1.begin(); // Initialisation
}

void loop() {
    delay(30);
    player1.update(); // read key state
    if (player1.button_A() && player1.button_D_down()
        || player1.button_Start()) { // has no deeper meaning ;)
        int xachse = player1.axis_x(); // can be negative oder positive
                                   // regarding to orientation of the analog stick
    }
    // …
}
```

## Wireing

To use, hook up the following to the Arduino:
Digital I/O 2: N64 serial line
All appropriate grounding and power lines, i.e.
GND to left N64 controller PIN, Dig.PIN2 to middle Serial/Signal,
3.3V to right N64 PIN

```
   /------------\
  / O    O     O \
 | GND Signl 3.3V |
 |________________|
```

Maybe: connect PIN X with external 1K pull-up resistor to the 3.3V rail
Default and fallback PIN is 2

## Authors

Gamecube controller to Nintendo 64 adapter
 by Andrew Brown
Rewritten for N64 to HID by Peter Den Hartog
Modified to be a library with selectable pins by Kai Lüke
