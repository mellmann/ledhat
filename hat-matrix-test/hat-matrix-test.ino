
#include <Arduino.h>

#include <ESP32Encoder.h>
#include <ezButton.h>  // the library to use for SW pin

// IMU
#include <Adafruit_BNO055.h>

#define CLK_PIN 5  // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  18 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  19 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin

// LED
#include <FastLED.h>

#define LED_DATAPIN    13

#include "HatMatrix.h"


//Encoder encoder(DT_PIN, CLK_PIN);
ESP32Encoder encoder;

ezButton button(SW_PIN);

Adafruit_BNO055 bno;


#define NUMPIXELS     600 // number of LEDs in strip
#define MATRIX_WIDTH  69  // number of pixes in a line of the virtual matrix
#define MATRIX_HEIGHT 18  // number of line in the virtual matrix

typedef HatMatrix<NUMPIXELS, MATRIX_WIDTH, MATRIX_HEIGHT> MyHatMatrix;
MyHatMatrix matrix;


void setup() 
{  
  Serial.begin(115200);

  Serial.println("Initializing IMU...");
  if(!bno.begin()) {
      Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  } else {
    Serial.println("BNO055 connected!");
    delay(1000); // FIXME: do we need that?
    bno.setExtCrystalUse(true);
  }

  // encoder with the button
  encoder.attachSingleEdge ( DT_PIN, CLK_PIN );
  encoder.setCount ( 0 );
  button.setDebounceTime(50);  // set debounce time to 50 milliseconds

  // register the LED array
  FastLED.addLeds<WS2812B, LED_DATAPIN, GRB>(matrix.getLEDArray(), matrix.num_pixels());
  FastLED.setBrightness(64);
  FastLED.setDither(0);
}


int x = 0;
int y = 0;

void loop() 
{
  int counter = encoder.getCount();

  button.loop();
  int button_count = button.getCount();
  
  FastLED.clear();

  if(x != counter) {
    x = counter;
    Serial.print(x);
    Serial.print(", ");
    Serial.println(y);
  }

  if(y != button_count) {
    y = button_count;
    Serial.print(x);
    Serial.print(", ");
    Serial.println(y);
  }
  
  matrix.pixel(x, y) = CRGB(64, 0, 0);

  FastLED.show();
  FastLED.delay(25);
}
