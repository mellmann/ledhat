// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Arduino.h>

#include <ezButton.h>  // the library to use for SW pin
#include <ESP32Encoder.h>

#include <Adafruit_BNO055.h>

#define CLK_PIN 5  // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  18 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  19 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin


#include <FastLED.h>
// Here's how to control the LEDs from any two pins:
#define DATAPIN    13
//#define MAX_POWER_MILLIAMPS 500

#include "HatMatrix.h"
#include "Ball.h"

#define NUMPIXELS     600 // number of LEDs in strip
#define MATRIX_WIDTH  69  // number of pixes in a line of the virtual matrix
#define MATRIX_HEIGHT 18  // number of line in the virtual matrix

// dimensions of the hat matrix
// NOTE: The distance between the LEDs on the LED-strip is 16 mm.
//   The lines are placed with an offes of half a distance between the LEDs.
//   Between the LES we consider to be 'invisible' pixel.
//   This leads to the following matrix of pixels, with a horizontal distance of 8mm between the pixels
//     - LED-pixels:       [O]
//     - invisible pixels: [ ]
//       
//   [O][ ][O][ ][O][ ][O][ ]
//   [ ][O][ ][O][ ][O][ ][O]
//   [O][ ][O][ ][O][ ][O][ ]
//   [ ][O][ ][O][ ][O][ ][O]
//   [O][ ][O][ ][O][ ][O][ ]
//
// NOTE: maybe for future improvements: 8*3/2 = 12
const float px_dist_x = 8;  // mm
const float px_dist_y = 12; // mm

typedef HatMatrix<NUMPIXELS, MATRIX_WIDTH, MATRIX_HEIGHT> MyHatMatrix;
MyHatMatrix matrix(px_dist_x, px_dist_y);


// fire animation
SpaceBalls<MyHatMatrix> spaceBalls(matrix);


//Encoder encoder(DT_PIN, CLK_PIN);
ESP32Encoder encoder;
ezButton button(SW_PIN);
Adafruit_BNO055 bno;

void setup() 
{  
  Serial.begin(115200);
  
  //encoder.attachFullQuad ( DT_PIN, CLK_PIN );
  encoder.attachSingleEdge ( DT_PIN, CLK_PIN );
  encoder.setCount ( 0 );
  
  Serial.write("Initializing IMU...");
  if(!bno.begin()) {
      Serial.write("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  } else {
    Serial.write("BNO055 connected!");
    delay(1000); // FIXME: ist das nötig?
    bno.setExtCrystalUse(true);
  }
  
  button.setDebounceTime(50);  // set debounce time to 50 milliseconds

  FastLED.addLeds<WS2812B, DATAPIN, GRB>(matrix.getLEDArray(), matrix.num_pixels());
  FastLED.setBrightness(255);
  FastLED.setDither(BINARY_DITHER);
  //FastLED.setDither(DISABLE_DITHER);
  //FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
}


float last_angle = 0;
float angle_diff = 0; // angle movement since last update

void calculate_angle() 
{
  // calculate angle
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  float new_angle = euler.x();
  angle_diff = new_angle - last_angle;
  if(angle_diff > 180) {
    angle_diff -= 360;
  } else if(angle_diff < -180) {
    angle_diff += 360;
  }

  last_angle = new_angle;
  //Serial.println(last_angle);
}


int getEncoder() 
{
  const int maxCounter = 18;
  int counter = encoder.getCount();
  if(counter < 0) {
    counter = 0;
    encoder.setCount(counter);
  } else if(counter >= maxCounter) {
    counter = maxCounter;
    encoder.setCount(counter);
  }
  //Serial.println(counter);
  return counter;
}

float t = 0;
void loop() 
{
  t += 3.0;
  button.loop();
  calculate_angle();
  
  int counter = getEncoder();

  int state = button.getCount() % 2;

  FastLED.clear();

  /*
  for (int i = 0; i < NUMPIXELS; ++i) 
  {
    float v = 10 + (1.0+sin((coords_mm[i].y-t)/10.0))*45;
    float c = (1.0+sin(t / 100.0))*128;
    leds[i] = CHSV(c, 255, v);
  }
  */

  switch(state) {
    case 0: 
      spaceBalls.draw_snow(counter*10); 
      spaceBalls.update_imu(angle_diff); 
      break;
    case 1: 
      spaceBalls.draw_snow(counter*10); 
      spaceBalls.update_bouncing(); 
      break;
  }
  
  
  
  FastLED.show();
  FastLED.delay(10);
}
