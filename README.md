# Arduino N64 Controller Library

Based on the work in [this guide for using Nintendo controllers with Arduinos](http://www.instructables.com/id/Use-an-Arduino-with-an-N64-controller/) here comes a comfortable library for usage with, e.g., Arduino Uno and similar CPUs. For NES there is already [nespad](http://code.google.com/p/nespad/). This library uses inline assembly. Controllers can be attached to PIN 0 up to 13. But be aware that it's not written in best way possible. To use it, search for N64 in the Library Manager. Otherwise place the folder N64Controller into your `libraries` folder or download and import it as ZIP.

I used it in combination with [TVout](http://code.google.com/p/arduino-tvout/) and [EEPROM](http://arduino.cc/playground/Code/EEPROMWriteAnything) for highscore saving when I modified an existing Tetris port. The port used a Simple Tetris Clone under MIT license and if you want you can see the result here which is quite nice: [N64Tetris](http://pothos.blogsport.eu/files/2012/03/N64Tetris.zip).


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

* Digital I/O PIN specified as parameter: Connect to N64 middle serial/signal PIN (Arduino PIN 2 is the fallback if you specify a PIN > 13)
* Grounding GND: Connect to left N64 controller PIN
* Power line 3.3V: Connect to right N64 controller PIN

```
   /------------\
  / O    O     O \
 | GND Signl 3.3V |
 |________________|
```

Maybe: connect PIN X with external 1K pull-up resistor to the 3.3V rail

## Authors

* Gamecube controller to Nintendo 64 adapter by Andrew Brown
* Rewritten for N64 to HID by Peter Den Hartog
* Modified to be a library with selectable pins by Kai Lüke
