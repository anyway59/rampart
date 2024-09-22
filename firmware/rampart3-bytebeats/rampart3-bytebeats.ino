/*
   Rampart Bytebeats @copyright 2024, Mark Washeim <blueprint@poetaster.de>
   GPLv3, see LICENSE
   some parts use other licensces (nyblybyte.h is MIT)
   and some parts are Public Domain in as far as permissible by law

   a number of formulas https://raw.githubusercontent.com/schollz/nyblcore/main/bytebeat/bytebeat.ino

   Many contributions from the internet :) See nyblybyte.h for many equations origins and original form.
*/

#include <EEPROM.h>
#include <EncoderButton.h>
// for pwm init functions
const unsigned int TOP = 0x07FF; // 11-bit resolution.  7812 Hz PWM

// noise reduction for cv reads
uint16_t potvalue[3]; // pot readings
uint16_t lastpotvalue[3]; // old pot readings
#define POT_SAMPLE_TIME 30 // delay time between pot reads
#define MIN_POT_CHANGE 25 // locked pot reading must change by this in order to register
#define MIN_COUNTS 20  // unlocked pot must change by this in order to register
#define POT_AVERAGING 3 // analog sample averaging count 
#define POT_MIN 4   // A/D may not read min value of 0 so use a bit larger value for map() function
#define POT_MAX 1019 // A/D may not read max value of 1023 so use a bit smaller value for map() function

#define LEDPIN   13 // usually 13
#define PWMPIN 11

#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#include <SSD1306init.h>
#define I2C_ADDRESS 0x3C
SSD1306AsciiAvrI2c display;



long t = 0;
volatile int a, b, c, i, offA, offB;
volatile int result;
int d = 0; // hmm?

int freqmod = 0;

bool awaitingSync = true;
bool acceptEncoder = false;

byte prog = 1;
byte bank = 1;

byte pb1total = 18;

byte pb2total = 18;

byte pb3total = 16;

byte pb4total = 7;

byte bb_sound = 1;

byte numSounds = (pb1total + pb2total + pb3total + pb4total);
bool bb_running = false;

// these ranges are provisional and in schollz equations need to be reset
volatile int aMax = 99, aMin = 0, bMax = 99, bMin = 0, cMax = 99, cMin = 0;

// default rate close to the original bytebeat speed
int SRATE = 8192; // 16384;

bool debug = true;

// encoder
// the a and b + the button pin large encoders are 6,5,4
EncoderButton eb1(5, 6, 4);

// the library has nice button handling and uses bounce2
EncoderButton left(3); // pin 3 rampart
EncoderButton right(2); // pin 2 rampart

int encoder_pos_last = 0;
long encoder_delta = 0;
int enc_offset = 1; // changes direction
int enc_delta; // which direction


#include "bytebeats.h" // forwards

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
  if (bb_running) {
    cli();
    bb_running = false;
  }
  else {
    pwmSetup();
    bb_running = true;
    EEPROM.write(0, bb_sound);
    if (debug) {
    Serial.print("Saving bb_sound to EEPROM: ");
    Serial.println(bb_sound);
    }
  }

  if (debug) {
    Serial.print("button1 longPressCount: ");
    Serial.println(eb.longPressCount());
  }
  // set which bank to select formulas from
  /*
  bank = ((bank + 1) % 3) + 1;

  if (debug) {
    Serial.print("New bank: ");
    Serial.println(bank);
  }
  */

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


  if (debug) {
    Serial.print("click count: ");
    Serial.println(eb.clickCount());
  }
  // displayUpdate();
}

/**
    handle right button long press
*/
void onRightLongPress(EncoderButton& right) { 

  if (debug) {
    Serial.println("Right button longPress");
    }
    // set which bank to select formulas from
 
  //bank = bank + 1;
  //if (bank > 3) {bank = 1;}

  //displaySound();
 }

 /**
    handle left button long press
*/
void onLeftLongPress(EncoderButton& left) { 

    // set which bank to select formulas from
   if (debug) {
    Serial.println("Left button longPress");
    } 
  //bank = bank - 1;
  //if (bank < 1) {bank = 3;}

  //displaySound();
  }

/**
    handle left button short release
*/
void onLeftReleased(EncoderButton& left) {
  
   if (debug) {
    Serial.println("Left button released");
    } 
 


  //displaySound();
}





/**
    handle right button short release
*/
void onRightReleased(EncoderButton& right) {

    if (debug) {
    Serial.println("Right button released");
    } 


}




/**
   A function to handle the 'encoder' event without button
*/
void onEb1Encoder(EncoderButton& eb) {

  //displayUpdate();
  encoder_delta = eb.increment();
  if (debug) {
    Serial.print("eb1 incremented by: ");
    Serial.println(encoder_delta);
    //Serial.print("eb1 position is: ");
    //Serial.println(SRATE);
  }
  int newbb_sound = bb_sound;
  if (acceptEncoder) {
    
    if (encoder_delta < 0 ) {
    newbb_sound = bb_sound - 1;
    if (newbb_sound < 1) {
        newbb_sound = numSounds;
       }
       acceptEncoder = false;
     }  
     else
    {
      newbb_sound = bb_sound + 1; 
       if (newbb_sound > numSounds) {
       newbb_sound = 1;
       } 
    } 
  }
  else
  {
    acceptEncoder = true;
  }

  bb_sound = newbb_sound;
  calculateProgAndBank();

  /*
  long cstep = eb.increment() * 64;
  long oSRATE = SRATE;
  SRATE = SRATE + cstep;

  if (SRATE != oSRATE) {
    OCR1A = F_CPU / SRATE;
  }
  */
   if ( bb_running ) {
    EEPROM.write(0, bb_sound);
    if (debug) {
    Serial.print("Saving bb_sound to EEPROM: ");
    Serial.println(bb_sound);
    }
   }


  displaySound();
}

// values to compare input on CV pins
int lastA = 0;
int lastB = 0;
int lastC = 0;


void setup() {

  if (debug) {
    Serial.begin(57600);
    Serial.println(F("Started"));
  }

 display.begin(&Adafruit128x64, I2C_ADDRESS);
 display.setFont(Adafruit5x7);
 display.clear();
    
 display.set2X();
 display.println("");
 display.println("Welcome to");
 display.println("BYTEBEATS");
 delay(3000);
 display.clear();

 byte savedsound = EEPROM.read(0);
 if(debug) { 
      Serial.print("Retrieved selection from EEPROM: ");
      Serial.print(savedsound);
 }
 if ((savedsound > 0) && (savedsound <= numSounds)) {
  bb_sound = savedsound;
 }

 

  pinMode(LEDPIN, OUTPUT);
  pinMode(PWMPIN, OUTPUT);
  
  //lastA =  analogRead(A3);
  //lastB =  analogRead(A6);
  //lastC =  analogRead(A7);
  
  //pwmSetup();

  //Link the event(s) to your function
  eb1.setClickHandler(onEb1Clicked);
  eb1.setEncoderHandler(onEb1Encoder);
  eb1.setLongPressHandler(onEb1LongPress, true);


  eb1.setEncoderPressedHandler(onEb1PressTurn);

  // program up/down buttons
  left.setReleasedHandler(onLeftReleased);
  left.setLongPressHandler(onLeftLongPress, true);

  left.setRateLimit(7);
  right.setReleasedHandler(onRightReleased);
  right.setLongPressHandler(onRightLongPress, true);
  right.setRateLimit(6);
  eb1.setRateLimit(6);

  calculateProgAndBank();

  displaySound();

}

void displaySound() {

  delay(1000);
  display.setCursor(0,0); 
  display.print("BANK: ");
  display.print(bank);
  display.println("  ");
  display.print("PROG: ");
  display.print(prog);
  display.println("  ");

}

void calculateProgAndBank() {
  if ( bb_sound < 1)   {
    bank = 1;
    prog = 1;
  }
  else if ( bb_sound < (pb1total + 1)  ) {
    bank = 1;
    prog = bb_sound;
  }
  else if (  bb_sound < (pb1total + pb2total + 1)  )  {
    bank = 2;
    prog = bb_sound - pb1total;
  }
  else if (  bb_sound < (pb1total + pb2total + pb3total + 1)  )  {
    bank = 3;
    prog = bb_sound - (pb1total + pb2total);
  } 
  else if (  bb_sound < (pb1total + pb2total + pb3total + pb4total + 1)  )  {
    bank = 4;
    prog = bb_sound - (pb1total + pb2total + pb3total);
  }
  else {
    bank = 1;
    prog = 1;
  }


}


void updateControl() {

  // this was old code from the midi-boy bytebeats sketch
  // tempo = map( analogRead(TEMPO), 0, 1023, 1, 256);
  // iterations = map( analogRead(ITERATIONS), 0, 1023, 16, 256);
  
  // EncoderButton object updates
  eb1.update();
  left.update();
  right.update();
  
  if (millis() > timeoffset + 50 ) {
    timeoffset = millis();
    //display_value(SRATE);
    if (result > 5000) digitalWrite(LEDPIN, HIGH);
    if (result < 1000) digitalWrite(LEDPIN, LOW);

  }



}

void loop() {
  updateControl(); // required here
  adc();
  knobs();


  
}



// used for mapping adc input on pins 5 - 7 to a - c
void adc() {
  // this is just very wrong ;)
  // take the average on the input on pin 3,6,7
  uint16_t A =  map(readcv(0), 0, 1023, aMin, aMax);
  uint16_t B =  map(readcv(1), 0, 1023, aMin, aMax);
  uint16_t C =  map(readcv(2), 0, 1023, aMin, aMax);
  
    if (lastA != A) {
      lastA = A;
      offA = lastA / 2;
      if(debug) Serial.print("A: ");
      if(debug) Serial.println(a);
    }

    if (lastB != B) {
      lastB = B;
      offB = lastB / 2;
      if(debug) Serial.print("B: ");
      if(debug) Serial.println(b);
    }

    if (lastC != C) {
      lastC = C;
      //offC = lastC / 2 ;
      //if(debug) Serial.print("C: ");
      //if(debug) Serial.println(c);
    }



}

uint16_t readcv(uint8_t potnum) {
  int val = 0;
  int input;
  switch (potnum) { // map potnum to pin
    case 0:
      input = A3;
      break;
    case 1:
      input = A6;
      break;
    case 2:
      input = A7;
      break;
  }
  // note that Pikocore pots are wired "backwards" - max voltage is full ccw
  for (int j = 0; j < POT_AVERAGING; ++j) val += (analogRead(input)); // read the A/D a few times and average for a more stable value
  val = val / POT_AVERAGING;
 
  if (abs(lastpotvalue[potnum] - val) > MIN_COUNTS ) { 
    lastpotvalue[potnum] = val; // even if pot is unlocked, make sure pot has moved at least MIN_COUNT counts so values don't jump around

    if(debug) { 
     if (potnum != 2)  {
      Serial.print("pot ");
      Serial.print(potnum);
      Serial.print(" readcv: ");
      Serial.println(val); 
     }
    }
    if (potnum == 2)  {        
        if (val > 80) {
          if (awaitingSync ) {
            // sync event
            freqmod = (( freqmod+1) % 8);
            OCR1A = F_CPU / (SRATE - (freqmod * 200));
            if ((freqmod%2) == 0) { t = 0; }
            enc_offset = (freqmod % 7) + 1;
            if ((freqmod % 7) == 0) { enc_offset = enc_offset * -1;}
            if ((freqmod%3) == 0) {offB = 50*freqmod;}


            

            //if(debug) { 
            //Serial.print("sync? : ");
            //Serial.println(val);
            //}
          }
          awaitingSync = false;
        }
        else {
          awaitingSync = true;
        }
      }

    }  else {
    val = lastpotvalue[potnum];
  }
    
 potvalue[potnum] = val; // pot is unlocked so save the reading

  return val;
}

// pot inputs
void knobs() {
  a = map(analogRead(A0), 0, 1023, aMin, aMax) + offA;
  b = map(analogRead(A1), 0, 1023, bMin, bMax) + offB;
  c = map(analogRead(A2), 0, 1023, cMin, cMax);
}


// show a number in binary on the LEDs
void display_value(int16_t value) {
  for (int i = 7; i >= 0; i--) { // NOPE + 1 can loop this way because port assignments are sequential
    //digitalWrite(led[i], result & 1);
    value = value >> 1;
  }
  //display_timer = millis();
}
