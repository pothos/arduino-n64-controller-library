
#include "N64Controller.h"
#include <Arduino.h>
#include "pins_arduino.h"

#define NOP asm volatile ("nop")
#define NOP5 asm volatile ("nop\nnop\nnop\nnop\nnop\n")
#define NOP30 asm volatile ("nop\nnop\nnop\nnop\nnop\n" \
                              "nop\nnop\nnop\nnop\nnop\n" \
                              "nop\nnop\nnop\nnop\nnop\n" \
                              "nop\nnop\nnop\nnop\nnop\n" \
                              "nop\nnop\nnop\nnop\nnop\n" \
                              "nop\nnop\nnop\nnop\nnop\n")

// these two macros set arduino pin 2 to input or output, which with an
// external 1K pull-up resistor to the 3.3V rail, is like pulling it high or
// low.  These operations translate to 1 op code, which takes 2 cycles
#define N64_PIND_HIGH DDRD &= ~pincode
#define N64_PIND_LOW DDRD |= pincode
#define N64_PIND_QUERY (PIND & pincode)

#define N64_PINB_HIGH DDRB &= ~pincode
#define N64_PINB_LOW DDRB |= pincode
#define N64_PINB_QUERY (PINB & pincode)

N64Controller::N64Controller() {
}

N64Controller::N64Controller(int serialPin) {
  n64_PIN = serialPin;
}
void N64Controller::begin(int serialPin) {
  n64_PIN = serialPin;
  begin();
}
void N64Controller::begin() {
  // Communication with N64 controller controller on this pin
  // Don't remove these lines, we don't want to push +5V to the controller
  digitalWrite(n64_PIN, LOW);
  pinMode(n64_PIN, INPUT);
  n64_first_register = true;
  switch (n64_PIN) {
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
            n64_pincode = 0x04; n64_PIN = 2;
            break;
  }
  if (n64_first_register) {
    N64_init_PIND(n64_pincode);
  } else {
    N64_init_PINB(n64_pincode);
  }
}

void N64Controller::N64_init_PIND(char pincode) {
  // Initialize the gamecube controller by sending it a null byte.
  // This is unnecessary for a standard controller, but is required for the
  // Wavebird.
  unsigned char initialize = 0x00;
  noInterrupts();
  N64_PIND_send(pincode, &initialize, 1);

  // Stupid routine to wait for the gamecube controller to stop
  // sending its response. We don't care what it is, but we
  // can't start asking for status if it's still responding
  int x;
  for (x=0; x<64; x++) {
      // make sure the line is idle for 64 iterations, should
      // be plenty.
      if (!N64_PIND_QUERY)
          x = 0;
  }

  // Query for the gamecube controller's status. We do this
  // to get the 0 point for the control stick.
  unsigned char command[] = {0x01};
  N64_PIND_send(pincode, command, 1);
  // read in data and dump it to N64_raw_dump
  N64_PIND_get(pincode);
  interrupts();
}

void N64Controller::N64_init_PINB(char pincode) {
  unsigned char initialize = 0x00;
  noInterrupts();
  N64_PINB_send(pincode, &initialize, 1);
  int x;
  for (x=0; x<64; x++) {
      if (!N64_PINB_QUERY)
          x = 0;
  }
  unsigned char command[] = {0x01};
  N64_PINB_send(pincode, command, 1);
  N64_PINB_get(pincode);
  interrupts();
}

/**
 * This sends the given byte sequence to the controller
 * length must be at least 1
 * Oh, it destroys the buffer passed in as it writes it
 */
void N64Controller::N64_PIND_send(char pincode, unsigned char *buffer, char length) {
    // Send these bytes
    char bits;

    // This routine is very carefully timed by examining the assembly output.
    // Do not change any statements, it could throw the timings off
    //
    // We get 16 cycles per microsecond, which should be plenty, but we need to
    // be conservative. Most assembly ops take 1 cycle, but a few take 2
    //
    // I use manually constructed for-loops out of gotos so I have more control
    // over the outputted assembly. I can insert nops where it was impossible
    // with a for loop
    
    asm volatile (";Starting outer for loop");
outer_loop:
    {
        asm volatile (";Starting inner for loop");
        bits=8;
inner_loop:
        {
            // Starting a bit, set the line low
            asm volatile (";Setting line to low");
            N64_PIND_LOW; // 1 op, 2 cycles

            asm volatile (";branching");
            if (*buffer >> 7) {
                asm volatile (";Bit is a 1");
                // 1 bit
                // remain low for 1us, then go high for 3us
                // nop block 1
                NOP5;
                
                asm volatile (";Setting line to high");
                N64_PIND_HIGH;

                // nop block 2
                // we'll wait only 2us to sync up with both conditions
                // at the bottom of the if statement
                NOP30;

            } else {
                asm volatile (";Bit is a 0");
                // 0 bit
                // remain low for 3us, then go high for 1us
                // nop block 3
                NOP30; NOP5; NOP;

                asm volatile (";Setting line to high");
                N64_PIND_HIGH;

                // wait for 1us
                asm volatile ("; end of conditional branch, need to wait 1us more before next bit");
                
            }
            // end of the if, the line is high and needs to remain
            // high for exactly 16 more cycles, regardless of the previous
            // branch path

            asm volatile (";finishing inner loop body");
            --bits;
            if (bits != 0) {
                // nop block 4
                // this block is why a for loop was impossible
                NOP5; NOP; NOP; NOP; NOP;

                // rotate bits
                asm volatile (";rotating out bits");
                *buffer <<= 1;

                goto inner_loop;
            } // fall out of inner loop
        }
        asm volatile (";continuing outer loop");
        // In this case: the inner loop exits and the outer loop iterates,
        // there are /exactly/ 16 cycles taken up by the necessary operations.
        // So no nops are needed here (that was lucky!)
        --length;
        if (length != 0) {
            ++buffer;
            goto outer_loop;
        } // fall out of outer loop
    }

    // send a single stop (1) bit
    // nop block 5
    asm volatile ("nop\nnop\nnop\nnop\n");
    N64_PIND_LOW;
    // wait 1 us, 16 cycles, then raise the line 
    // 16-2=14
    // nop block 6
    NOP5; NOP5; NOP; NOP; NOP; NOP;

    N64_PIND_HIGH;

}


void N64Controller::N64_PINB_send(char pincode, unsigned char *buffer, char length) {
    char bits;
    asm volatile (";Starting outer for loop");
outer_loop:
    {
        asm volatile (";Starting inner for loop");
        bits=8;
inner_loop:
        {
            asm volatile (";Setting line to low");
            N64_PINB_LOW;
            asm volatile (";branching");
            if (*buffer >> 7) {
                asm volatile (";Bit is a 1");
                NOP5;
                asm volatile (";Setting line to high");
                N64_PINB_HIGH;
                NOP30;
            } else {
                asm volatile (";Bit is a 0");
                NOP30; NOP5; NOP;

                asm volatile (";Setting line to high");
                N64_PINB_HIGH;
                asm volatile ("; end of conditional branch, need to wait 1us more before next bit");
            }
            asm volatile (";finishing inner loop body");
            --bits;
            if (bits != 0) {
                NOP5; NOP; NOP; NOP; NOP;
                asm volatile (";rotating out bits");
                *buffer <<= 1;
                goto inner_loop;
            }
        }
        asm volatile (";continuing outer loop");
        --length;
        if (length != 0) {
            ++buffer;
            goto outer_loop;
        }
    }
    NOP; NOP; NOP; NOP;
    N64_PINB_LOW;
    NOP5; NOP5; NOP; NOP; NOP; NOP;
    N64_PINB_HIGH;
}


void N64Controller::N64_PIND_get(char pincode)
{
    // listen for the expected 8 bytes of data back from the controller and
    // blast it out to the N64_raw_dump array, one bit per byte for extra speed.
    // Afterwards, call translate_raw_data() to interpret the raw data and pack
    // it into the N64_status struct.
    asm volatile (";Starting to listen");
    unsigned char timeout;
    char bitcount = 32;
    char *bitbin = N64_raw_dump;

    // Again, using gotos here to make the assembly more predictable and
    // optimization easier (please don't kill me)
read_loop:
    timeout = 0x3f;
    // wait for line to go low
    while (N64_PIND_QUERY) {
        if (!--timeout)
            return;
    }
    // wait approx 2us and poll the line
    NOP30;
    *bitbin = N64_PIND_QUERY;
    ++bitbin;
    --bitcount;
    if (bitcount == 0)
        return;

    // wait for line to go high again
    // it may already be high, so this should just drop through
    timeout = 0x3f;
    while (!N64_PIND_QUERY) {
        if (!--timeout)
            return;
    }
    goto read_loop;

}



void N64Controller::N64_PINB_get(char pincode)
{
    asm volatile (";Starting to listen");
    unsigned char timeout;
    char bitcount = 32;
    char *bitbin = N64_raw_dump;
read_loop:
    timeout = 0x3f;
    while (N64_PINB_QUERY) {
        if (!--timeout)
            return;
    }
    NOP30;
    *bitbin = N64_PINB_QUERY;
    ++bitbin;
    --bitcount;
    if (bitcount == 0)
        return;
    timeout = 0x3f;
    while (!N64_PINB_QUERY) {
        if (!--timeout)
            return;
    }
    goto read_loop;
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
  if (n64_first_register) {
    noInterrupts();
    N64_PIND_send(n64_pincode, command, 1);
    N64_PIND_get(n64_pincode);
    interrupts();
  } else {
    noInterrupts();
    N64_PINB_send(n64_pincode, command, 1);
    N64_PINB_get(n64_pincode);
    interrupts();
  }
}
