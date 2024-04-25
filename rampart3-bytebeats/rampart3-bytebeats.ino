/*
   Rampart Bytebeats @copyright 2024, Mark Washeim <blueprint@poetaster.de>
   GPLv3, see LICENSE
   some parts use other licensces (nyblybyte.h is MIT)
   and some parts are Public Domain in as far as permissible by law

   a number of formulas https://raw.githubusercontent.com/schollz/nyblcore/main/bytebeat/bytebeat.ino

   Many contributions from the internet :) See nyblybyte.h for many equations origins and original form.
*/


#include <EncoderButton.h>
//#include <Bounce2.h>
//#include <Adafruit_SSD1306.h> // conflicts with timer.
#include <EncoderButton.h>

// for pwm init functions
const unsigned int TOP = 0x07FF; // 11-bit resolution.  7812 Hz PWM

#define LEDPIN   13 // usually 13
#define PWMPIN 11

long t = 0;
volatile int a, b, c, i;
volatile int result;
int d = 0; // hmm?

int prog = 1;
int bank = 1;
int pb1 = 1;
int pb1total = 13;
int pb2 = 1;
int pb2total = 19;
int pb3 = 1;
int pb3total = 21;
int numProg = 52;

// these ranges are provisional and in schollz equations need to be reset
volatile int aMax = 99, aMin = 0, bMax = 99, bMin = 0, cMax = 99, cMin = 0;

// default rate close to the original bytebeat speed
int SRATE = 8192; // 16384;

bool debug = true;

// encoder
// the a and b + the button pin
EncoderButton eb1(6, 5, 4);

// the library has nice button handling and uses bounce2
EncoderButton left(3); // pin 3 rampart
EncoderButton right(2); // pin 2 rampart

int encoder_pos_last = 0;
long encoder_delta = 0;
int enc_offset = 1; // changes direction
int enc_delta; // which direction


#include "bytebeats.h" // forwards

// unused screen stuff. keep for now.

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//#include "font.h"
//#define myfont Org_01  // sigh
const int oled_sda_pin = 20;
const int oled_scl_pin = 21;
const int oled_i2c_addr = 0x3C;


// some constants for sound production these are the old ones.
//counter for sound
float floatI = 0;
float tempo = 16;
int currentSound = 4;
int iterations = 256;

// analog freq pins OLD
#define TEMPO 0
#define ITERATIONS 1

long timeoffset = 0;

/**
   handle encoder button long press event
*/
void onEb1LongPress(EncoderButton& eb) {

  if (debug) {
    Serial.print("button1 longPressCount: ");
    Serial.println(eb.longPressCount());
  }
}
/**
   handle encoder turn with  button pressed
   offsets OCR2A
*/
void onEb1PressTurn(EncoderButton& eb) {
  enc_delta = eb.increment();

  int dir = enc_offset + eb.increment();
  dir = constrain(dir, -7, 7 );

  enc_offset = dir;
  if (debug) {
    Serial.print("eb1 press inc by: ");
    Serial.println(eb.increment());
    Serial.print("enc_offset is: ");
    Serial.println(enc_offset);
  }
}

/**
   handle encoder turn with  button pressed
*/
void onEb1Clicked(EncoderButton& eb) {

  // set which bank to select formulas from
  bank = eb.clickCount();

  if (debug) {
    Serial.print("bank: ");
    Serial.println(eb.clickCount());
  }
  // displayUpdate();
}

/**
    handle left button short release
*/
void onLeftReleased(EncoderButton& left) {

  if (bank == 1)
  {
    if (pb1 > 1) {
      pb1--;
    } else if (pb1 == 1) {
      pb1 = pb1total;
    }
    prog = pb1;
  } 
  else if (bank == 2) {
    if (pb2 > 1) {
      pb2--;
    } else if (pb2 == 1) {
      pb2 = pb2total;
    }
    prog = pb2;
  } 
  else if (bank == 3) {
    if (pb3 > 1) {
      pb3--;
    } else if (pb3 == 1) {
      pb3 = pb3total;
    }
    prog = pb3;
  }

  if (debug) {
    Serial.print("PROGRAM: ");
    Serial.println(prog);
  }
}

/**
    handle right button short release
*/
void onRightReleased(EncoderButton& right) {
  
  if (bank == 1)
  {
    if (pb1 < pb1total) {
      pb1++;
    } else if (pb1 == pb1total) {
      pb1 = 1;
    }
    prog = pb1;
  } 
  else if (bank == 2) {
    if (pb2 < pb2total) {
      pb2++;
    } else if (pb2 == pb2total) {
      pb2 = 1;
    }
    prog = pb2;
  } 
  else if (bank == 3) {
    if (pb3 < pb2total) {
      pb3++;
    } else if (pb3 == pb3total) {
      pb3 = 1;
    }
    prog = pb3;
  }
  if (debug) {
    Serial.print("PROGRAM: ");
    Serial.println(prog);
  }
}

/**
   A function to handle the 'encoder' event without button
*/
void onEb1Encoder(EncoderButton& eb) {

  //displayUpdate();
  encoder_delta = eb.increment();
  long cstep = eb.increment() * 64;
  long oSRATE = SRATE;
  SRATE = SRATE + cstep;

  if (SRATE != oSRATE) {
    OCR1A = F_CPU / SRATE;
  }

  if (debug) {
    Serial.print("eb1 incremented by: ");
    Serial.println(eb.increment());
    Serial.print("eb1 position is: ");
    Serial.println(SRATE);
  }
}


void setup() {

  if (debug) {
    Serial.begin(9600);
    Serial.println(F("Started"));
  }

  pinMode(LEDPIN, OUTPUT);
  pinMode(PWMPIN, OUTPUT);

  pwmSetup();

  //Link the event(s) to your function
  eb1.setClickHandler(onEb1Clicked);
  eb1.setEncoderHandler(onEb1Encoder);
  eb1.setLongPressHandler(onEb1LongPress, true);
  eb1.setEncoderPressedHandler(onEb1PressTurn);

  // program up/down buttons
  left.setReleasedHandler(onLeftReleased);
  left.setRateLimit(7);
  right.setReleasedHandler(onRightReleased);
  right.setRateLimit(6);


}


void updateControl() {

  // this was old code from the midi-boy bytebeats sketch
  // tempo = map( analogRead(TEMPO), 0, 1023, 1, 256);
  // iterations = map( analogRead(ITERATIONS), 0, 1023, 16, 256);

  if (millis() > timeoffset + 50 ) {
    timeoffset = millis();
    //display_value(SRATE);
    if (result > 5000) digitalWrite(LEDPIN, HIGH);
    if (result < 1000) digitalWrite(LEDPIN, LOW);
  }

  // EncoderButton object updates
  eb1.update();
  left.update();
  right.update();

}

void loop() {
  knobs();
  adc();
  updateControl(); // required here
}

// used for mapping adc input on pins 5 - 7 to a - c
void adc() {
  // this is just very wrong ;)
  // take the average on the input on pin 7
  b =  ( b + map(analogRead(6), 0, 1023, bMin, bMax) ) / 2;
  c =  ( c + map(analogRead(7), 0, 1023, cMin, cMax) ) / 2;
}

// pot inputs
void knobs() {
  a = map(analogRead(0), 0, 1023, aMin, aMax);
  b = map(analogRead(1), 0, 1023, bMin, bMax);
  c = map(analogRead(2), 0, 1023, cMin, cMax);
}


// show a number in binary on the LEDs
void display_value(int16_t value) {
  for (int i = 7; i >= 0; i--) { // NOPE + 1 can loop this way because port assignments are sequential
    //digitalWrite(led[i], result & 1);
    value = value >> 1;
  }
  //display_timer = millis();
}


// additional display methods (these don't clear)
//
// --- display details
//
typedef struct {
  int x;
  int y;
  const char* str;
} pos_t;
/*
  //// {x,y} locations of play screen items
  const int step_text_pos[] = { 0, 15, 16, 15, 32, 15, 48, 15, 64, 15, 80, 15, 96, 15, 112, 15 };
  const pos_t bpm_text_pos    = {.x=0,  .y=15, .str="bpm:%3d" };
  const pos_t trans_text_pos  = {.x=35, .y=15, .str="trs:%+2d" };
  const pos_t seqno_text_pos  = {.x=75, .y=15, .str="seq:%d" };
  const pos_t seq_info_pos    = {.x=60, .y=45, .str="" };
  const pos_t play_text_pos   = {.x=110,.y=57, .str="" };

  const pos_t oct_text_offset = { .x=3, .y=10,  .str="" };
  const pos_t gate_bar_offset = { .x=0, .y=-15, .str="" };
  const pos_t edit_text_offset= { .x=3, .y=22,  .str="" };
  const int gate_bar_width = 14;
  const int gate_bar_height = 4;

  void displayUpdate() {
  display.clearDisplay();
  //display.setFont(&myfont);
  display.setFont(u8g2_font_ncenB14_tr);
  //display.setTextColor(WHITE, 0);
  //display.setFont(&myfont2);
  // bpm
  display.setCursor(bpm_text_pos.x, bpm_text_pos.y);
  display.print("inc: ");
  display.print(eb1.increment());

  // transpose
  display.setCursor(trans_text_pos.x, trans_text_pos.y);
    display.print("pos: ");
  display.print(eb1.position());

  // seqno
  display.setCursor(seqno_text_pos.x, seqno_text_pos.y);
  display.print("clks: ");
  display.print(eb1.clickCount());  // user sees 1-8
  // seq info / meta
  display.setCursor(seq_info_pos.x, seq_info_pos.y);
  display.print((String) a);

  // play/pause
  //display.setCursor(play_text_pos.x, play_text_pos.y);
  //display.print(seqr.playing ? " >" : "[]");

  display.display();
  }



  void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
  }

  void print_cur(String Bytes, String Bytes2) {
  display.setCursor(seq_info_pos.x, seq_info_pos.y);
  display.print(Bytes);
  display.print(Bytes2);  // user sees 1-8
  display.display();
  }
*/
