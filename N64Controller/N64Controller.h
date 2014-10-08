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


class N64Controller {
  public:
    N64Controller();
    N64Controller(int serialPin); // first thing to call
    void begin(int serialPin);
    void begin();                     // second thing to call
    void update();                    // then update always and get button info
                                      // consider to have a delay instead of
                                      // calling update all the time in a loop
    inline bool button_D_up() { return n64_key_Dup; };
    inline bool button_D_down() { return n64_key_Ddown; };
    inline bool button_D_left() { return n64_key_Dleft; };
    inline bool button_D_right() { return n64_key_Dright; };
    inline bool button_Start() { return n64_key_Start; };
    inline bool button_A() { return n64_key_A; };
    inline bool button_B() { return n64_key_B; };
    inline bool button_Z() { return n64_key_Z; };
    inline bool button_L() { return n64_key_L; };
    inline bool button_R() { return n64_key_R; };
    inline bool button_C_up() { return n64_key_Cup; };
    inline bool button_C_down() { return n64_key_Cdown; };
    inline bool button_C_left() { return n64_key_Cleft; };
    inline bool button_C_right() { return n64_key_Cright; };
    inline int axis_x() { return n64_key_X; };
    inline int axis_y() { return n64_key_Y; };
    
    void print_N64_status();
  private:
    void set_up();
    int n64_PIN; // might also be set by constructor or begin()
    char n64_pincode;
    bool n64_first_register; // PIN0-7: DDRD PIN8-13: DDRB
    bool n64_key_Dup;
    bool n64_key_Ddown;
    bool n64_key_Dleft;
    bool n64_key_Dright;
    bool n64_key_Start;
    bool n64_key_Z;
    bool n64_key_A;
    bool n64_key_B;
    bool n64_key_Cup;
    bool n64_key_Cdown;
    bool n64_key_Cleft;
    bool n64_key_Cright;
    bool n64_key_L;
    bool n64_key_R;
    int n64_key_X;
    int n64_key_Y;
    
    void N64_init_PIND(char pincode);
    void N64_PIND_send(char pincode, unsigned char *buffer, char length);
    void N64_PIND_get(char pincode);
    
    void N64_init_PINB(char pincode);
    void N64_PINB_send(char pincode, unsigned char *buffer, char length);
    void N64_PINB_get(char pincode);
    
    void translate_raw_data();
    
    // 8 bytes of data that we get from the controller
    struct {
    // bits: 0, 0, 0, start, y, x, b, a
    unsigned char data1;
    // bits: 1, L, R, Z, Dup, Ddown, Dright, Dleft
    unsigned char data2;
    char stick_x;
    char stick_y;
    } N64_status;
    
    char N64_raw_dump[33]; // 1 received bit per byte
};

#endif
