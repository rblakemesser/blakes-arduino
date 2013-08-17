#include "LPD8806.h"
#include "SPI.h"

// Adapted from adafruit's awesome LPD8806 library, found here: https://github.com/adafruit/LPD8806 

/*****************************************************************************/

// Sequences
int numSequences = 7;
int currentSequence = 0;
int currentOK = HIGH;

// Number of RGB LEDs in strand:
int nLEDs = 14;
int dataPin  = 3;
int clockPin = 4;
int myButton = 0; // press button in pin D2, which is for some reason called 0 in interrupt land

volatile int buttonReading = LOW;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.
//LPD8806 strip = LPD8806(nLEDs);

void setup() {
  Serial.begin(9600); //logging
  attachInterrupt(myButton, stateChange, RISING);
  
  // Start up the LED strip
  strip.begin();
  // Update the strip, to start they are all 'off'
  strip.show();

}


void loop() {
  currentOK = HIGH;
  playSequence(currentSequence);
}

void stateChange() {
  buttonReading = !buttonReading;
  Serial.println("state changed!");
    if (buttonReading == HIGH) {
      currentOK = LOW;
      currentSequence += 1;
      if (currentSequence >= numSequences) {
        currentSequence = 0;
    }
  }
}

int playSequence(int current) {
  String logOne = "running sequence: ";
  String logStatement = logOne + current;
  Serial.println(logStatement);
  switch (current) {
     case 0:
       colorChase(strip.Color(  0,   0, 127), 50); // Blue
       colorChase(strip.Color(127,   0,   0), 50); // Red
       colorChase(strip.Color(127, 127,   0), 50); // Yellow
       colorChase(strip.Color(  0, 127,   0), 50); // Green
       colorChase(strip.Color(  0, 127, 127), 50); // Cyan
       colorChase(strip.Color(  0,   0, 127), 50); // Blue
       colorChase(strip.Color(127,   0, 127), 50); // Violet
       break;
       
     case 1: 
       colorWipe(strip.Color(127,   0,   0), 50);  // Red
       colorWipe(strip.Color(  0, 127,   0), 50);  // Green
       break;
       
     case 2:
       rainbowCycle(0);
       break;
       
     case 3:
       rainbow(10);
       break;
       
     case 4:
       redRainbow(10);
       break;
       
     case 5:
       blueRainbow(10);
       break;
       
     case 6:
       greenRainbow(10);
       break;
  }
}

void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
    if (currentOK == LOW) {
      return;
    }  
  }
}

void redRainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 128; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
    if (currentOK == LOW) {
      return;
    }  
  }
}

void blueRainbow(uint8_t wait) {
  int i, j;
   
  for (j=256; j < 384; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
    if (currentOK == LOW) {
      return;
    }  
  }
}

void greenRainbow(uint8_t wait) {
  int i, j;
   
  for (j=128; j < 256; j++) {     // 3 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
    if (currentOK == LOW) {
      return;
    }  
  }
}

// Fill the dots progressively along the strip.
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
    if (currentOK == LOW) {
      return;
    }
  }
}
// Chase one dot down the full strip.
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
    if (currentOK == LOW) {
      return;
    }  
  }

  strip.show(); // Refresh to turn off last pixel
}


// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
    if (currentOK == LOW) {
      return;
    }  
  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g -b - back to r

uint32_t Wheel(uint16_t WheelPos) {
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      r = 0;                  //red off
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      break; 
    case 2:
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      b = 127 - WheelPos % 128;  //blue down 
      break; 
  }
  return(strip.Color(r,g,b));
}
