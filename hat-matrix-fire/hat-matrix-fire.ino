
// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Arduino.h>

#include <ezButton.h>  // the library to use for SW pin
#include "Encoder.h"

#include <ESP32Encoder.h>

#include <Adafruit_BNO055.h>

#define CLK_PIN 5  // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  18 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  19 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin

//Encoder encoder(DT_PIN, CLK_PIN);
ESP32Encoder encoder;


ezButton button(SW_PIN);

Adafruit_BNO055 bno;

#include <FastLED.h>
// Here's how to control the LEDs from any two pins:
#define DATAPIN    13
#define NUMPIXELS 600 // Number of LEDs in strip

CRGB leds[ NUMPIXELS ];


const uint8_t kMatrixWidth = 69;
const uint8_t kMatrixHeight = 18;
uint16_t XYTable[kMatrixWidth * kMatrixHeight];

uint16_t XY( uint8_t x, uint8_t y)
{
  // any out of bounds address maps to the first hidden pixel
  if ( (x >= kMatrixWidth) || (y >= kMatrixHeight) ) {
    return NUMPIXELS;
  }

  uint16_t i = (y * kMatrixWidth) + x;
  uint16_t j = XYTable[i];
  return j;
}


void setup() 
{  

  // initialize the matrix values
  for(int i = 0; i < kMatrixWidth * kMatrixHeight; ++i) {
    XYTable[i] = (i % 2 == 0) ? i / 2 : NUMPIXELS;
  }
  
  //encoder.attachFullQuad ( DT_PIN, CLK_PIN );
  encoder.attachSingleEdge ( DT_PIN, CLK_PIN );
  encoder.setCount ( 0 );
  
  Serial.begin(115200);

  Serial.println("Initializing IMU...");
  if(!bno.begin()) {
      Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  } else {
    Serial.println("BNO055 connected!");
    delay(1000); // FIXME: do we need that?
    bno.setExtCrystalUse(true);
  }
  

  button.setDebounceTime(50);  // set debounce time to 50 milliseconds

  FastLED.addLeds<WS2812B, DATAPIN, GRB>(leds, NUMPIXELS);
  FastLED.setBrightness(64);
  FastLED.setDither(0);
}

int counter = 0;
int COOLING_BASE = 100;
int SPARKING_BASE = 120;

// Array of temperature readings at each simulation cell
uint8_t heat[kMatrixWidth][kMatrixHeight];

void Fire2012()
{
    int COOLING = COOLING_BASE + counter*10;
    int SPARKING = SPARKING_BASE - counter*10;
    
    // Step 1.  Cool down every cell a little
    for( int x = 0; x < kMatrixWidth; ++x) {
      for( int y = 0; y < kMatrixHeight; ++y) {
        heat[x][y] = qsub8( heat[x][y],  random8(0, ((COOLING * 10) / kMatrixHeight) + 2));
      }
    }
      
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int x = 0; x < kMatrixWidth; ++x) {
      for( int y = kMatrixHeight-1; y >= 2; y--) {
        heat[x][y] = (heat[x][y - 1] + heat[x][y - 2] + heat[x][y - 2] ) / 3;
      } 
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    for( int x = 0; x < kMatrixWidth; ++x) {
      if( random8() < SPARKING ) {
        int y = random8(2);
        heat[x][y] = qadd8( heat[x][y], random8(160,255) );
      }
    }
 
    // Step 4.  Map from heat cells to LED colors
    for( int x = 0; x < kMatrixWidth; ++x) {
      for( int y = 0; y < kMatrixHeight; ++y) {
        uint16_t idx = XY(x, (kMatrixHeight-1) - y);
        if(idx < NUMPIXELS) {
          CRGB color = HeatColor( heat[x][y]);
          leds[idx] = color;
        }
      }
    }
}

void loop() 
{
  counter = encoder.getCount();
  if(counter < 0) {
    counter = 0;
    encoder.setCount(counter);
  } else if(counter >= kMatrixHeight) {
    counter = kMatrixHeight;
    encoder.setCount(counter);
  }
  Serial.println((int)counter);

  FastLED.clear();

  Fire2012();


  //counter = (counter + 1) % kMatrixHeight;
  FastLED.show();
  FastLED.delay(25);
}
