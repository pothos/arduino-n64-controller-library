/**
 * Gamecube controller to Nintendo 64 adapter
 * by Andrew Brown
 * Rewritten for N64 to HID by Peter Den Hartog
 * Modified to be a library by Kai Lüke
 */

/**
 * To use, hook up the following to the Arduino:
 * Digital I/O 2: N64 serial line
 * All appropriate grounding and power lines, i.e.
 * GND to left N64 controller PIN, Dig.PIN2 to middle Serial/Signal,
 * 3.3V to right N64 PIN
 *    /------------\
 *   / O    O     O \
 *  | GND Signl 3.3V |
 *  |________________|
 * Maybe: connect PIN X with external 1K pull-up resistor to the 3.3V rail
 * Default and fallback PIN is 2
 */

#ifndef N64Controller_h
#define N64Controller_h

#define A_IDX       0
#define B_IDX       1
#define Z_IDX       2
#define START_IDX   3
#define D_UP_IDX    4
#define D_DOWN_IDX  5
#define D_LEFT_IDX  6
#define D_RIGHT_IDX 7
#define L_IDX       10
#define R_IDX       11
#define C_UP_IDX    12
#define C_DOWN_IDX  13
#define C_LEFT_IDX  14
#define C_RIGHT_IDX 15
#define X_IDX    16
#define Y_IDX    24

class N64Controller {
  public:
    N64Controller();
    N64Controller(int serialPin); // first thing to call
    void begin(int serialPin);
    void begin();                     // second thing to call
    void update();                    // then update always and get button info
                                      // consider to have a delay instead of
                                      // calling update all the time in a loop
    inline bool D_up() { return (N64_raw_dump[D_UP_IDX]) > 0; };
    inline bool D_down() { return (N64_raw_dump[D_DOWN_IDX]) > 0; };
    inline bool D_left() { return (N64_raw_dump[D_LEFT_IDX]) > 0; };
    inline bool D_right() { return (N64_raw_dump[D_RIGHT_IDX]) > 0; };
    inline bool Start() { return (N64_raw_dump[START_IDX]) > 0; };
    inline bool A() { return (N64_raw_dump[A_IDX]) > 0; };
    inline bool B() { return (N64_raw_dump[B_IDX]) > 0; };
    inline bool Z() { return (N64_raw_dump[Z_IDX]) > 0; };
    inline bool L() { return (N64_raw_dump[L_IDX]) > 0; };
    inline bool R() { return (N64_raw_dump[R_IDX]) > 0; };
    inline bool C_up() { return (N64_raw_dump[C_UP_IDX]) > 0; };
    inline bool C_down() { return (N64_raw_dump[C_DOWN_IDX]) > 0; };
    inline bool C_left() { return (N64_raw_dump[C_LEFT_IDX]) > 0; };
    inline bool C_right() { return (N64_raw_dump[C_RIGHT_IDX]) > 0; };
    inline char axis_x() { return axis(X_IDX); };
    inline char axis_y() { return axis(Y_IDX); };
    
    void print_N64_status();
  private:
    void set_up();
    int n64_PIN; // might also be set by constructor or begin()
    char n64_pincode;
    bool n64_first_register; // PIN0-7: DDRD PIN8-13: DDRB

    void N64_init_PIND(char pincode);
    void N64_PIND_send(char pincode, unsigned char *buffer, char length);
    void N64_PIND_get(char pincode);
    
    void N64_init_PINB(char pincode);
    void N64_PINB_send(char pincode, unsigned char *buffer, char length);
    void N64_PINB_get(char pincode);

    inline char axis(int index) {
      char value = 0;
      for (char i=0; i<8; i++) {
          value |= N64_raw_dump[index+i] ? (0x80 >> i) : 0;
      }
      return value;
    }

    char N64_raw_dump[33]; // 1 received bit per byte
};

#endif
