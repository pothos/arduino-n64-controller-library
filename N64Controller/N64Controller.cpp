
#include "N64Controller.h"
#include <Arduino.h>

N64Controller::N64Controller(unsigned char serialPin) {
  if(serialPin > 13)
    serialPin = 2;
  // Communication with N64 controller controller on this pin
  // Don't remove these lines, we don't want to push +5V to the controller
  digitalWrite(serialPin, LOW);
  pinMode(serialPin, INPUT);
  bool n64_first_register = true;
  char n64_pincode;
  switch (serialPin) {
    case 0: n64_pincode = 0x01;
            break;
    case 1: n64_pincode = 0x02;
            break;
    case 2: n64_pincode = 0x04;
            break;
    case 3: n64_pincode = 0x08;
            break;
    case 4: n64_pincode = 0x10;
            break;
    case 5: n64_pincode = 0x20;
            break;
    case 6: n64_pincode = 0x40;
            break;
    case 7: n64_pincode = 0x80;
            break;
    case 8: n64_pincode = 0x01; n64_first_register = false;
            break;
    case 9: n64_pincode = 0x02; n64_first_register = false;
            break;
    case 10: n64_pincode = 0x04; n64_first_register = false;
             break;
    case 11: n64_pincode = 0x08; n64_first_register = false;
             break;
    case 12: n64_pincode = 0x10; n64_first_register = false;
             break;
    case 13: n64_pincode = 0x20; n64_first_register = false;
             break;
    default:
            n64_pincode = 0x04;
            break;
  }
  if(n64_first_register) {
      interface = new N64Interface_PIND(n64_pincode);
  } else {
      interface = new N64Interface_PINB(n64_pincode);
  }
}

void N64Controller::begin() {
    interface->init();
}

void N64Controller::print_N64_status()
{
    // bits: A, B, Z, Start, Dup, Ddown, Dleft, Dright
    // bits: 0, 0, L, R, Cup, Cdown, Cleft, Cright
    Serial.println();
    Serial.print("Start: ");
    Serial.println(Start());

    Serial.print("Z:     ");
    Serial.println(Z());

    Serial.print("B:     ");
    Serial.println(B());

    Serial.print("A:     ");
    Serial.println(A());

    Serial.print("L:     ");
    Serial.println(L());
    Serial.print("R:     ");
    Serial.println(R());

    Serial.print("Cup:   ");
    Serial.println(C_up());
    Serial.print("Cdown: ");
    Serial.println(C_down());
    Serial.print("Cright:");
    Serial.println(C_right());
    Serial.print("Cleft: ");
    Serial.println(C_left());
    
    Serial.print("Dup:   ");
    Serial.println(D_up());
    Serial.print("Ddown: ");
    Serial.println(D_down());
    Serial.print("Dright:");
    Serial.println(D_right());
    Serial.print("Dleft: ");
    Serial.println(D_left());

    Serial.print("Stick X:");
    Serial.println(axis_x(), DEC);
    Serial.print("Stick Y:");
    Serial.println(axis_y(), DEC);
}

void N64Controller::update() {
  unsigned char command[] = {0x01};
  noInterrupts();
  interface->send(command, 1);
  interface->get();
  interrupts();
}
