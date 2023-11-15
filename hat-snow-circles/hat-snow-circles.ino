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

Encoder encoder(DT_PIN, CLK_PIN);
//ESP32Encoder encoder;


ezButton button(SW_PIN);

Adafruit_BNO055 bno;

#include <FastLED.h>
#define NUMPIXELS 600 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    13

// dimensions of the hat matrix
const float px_dist = 1.6;
const float px_per_cm = 34.5;
const float width = px_per_cm*px_dist;


CRGB leds[NUMPIXELS];

struct Point {
  float h;
  float a;
};

Point coords[NUMPIXELS];


void calculate_coords() 
{  
  float h = 0;
  float a = 0;
  coords[0].h = h;
  coords[0].a = a;
  
  for (int i = 1; i < NUMPIXELS; ++i) {
    h += 1.2 / px_per_cm;
    a += px_dist;
    
    if(a > width) {
      a -= width;
    }

    coords[i].h = h;
    coords[i].a = a;
  }
}

void setup() 
{  
  //encoder.attachFullQuad ( DT_PIN, CLK_PIN );
  //encoder.setCount ( 0 );
  
  Serial.begin(115200);

  Serial.write("Initializing IMU...");
  if(!bno.begin()) {
      Serial.write("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  } else {
    Serial.write("BNO055 connected!");
    delay(1000); // FIXME: ist das nötig?
    bno.setExtCrystalUse(true);
  }
  
  button.setDebounceTime(50);  // set debounce time to 50 milliseconds

  FastLED.addLeds<WS2812B, DATAPIN, GRB>(leds, NUMPIXELS);
  FastLED.setBrightness(255);
  FastLED.setDither(0);

  calculate_coords();
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
  Serial.println(last_angle);
}


float checkCircle(const Point& c, float x, float y) 
{
  float dx = (c.a - x);
  if (abs(dx) > width) dx -= (int)(dx/width)*width;
  
  if (dx > width/2) dx -= width;
  else if (dx < -width/2) dx += width;
  
  float dy = (c.h - y);
  
  float d = dx*dx + dy*dy;  

  return sqrt(d);
}



float t = 0.0;
float x = 8.0;
float y = 8.0;
float c = 0.0;

float vx = 0.8;
float vy = 0.1;

float s = 0;


void draw_circles() 
{
  float r = 5.0*s;
  for (int i = 0; i < NUMPIXELS; ++i) 
  {
    float d  = checkCircle(coords[i], x         ,y);
    float d1 = checkCircle(coords[i], x+width/2 ,y);
    if(d < r) {
      leds[i] = CHSV(c, 255, (d/r)*100); //CRGB(r*r - d, 0, 0);
    } else if(d1 < r) {
      leds[i] = CHSV(c, 255, (d1/r)*100); //CRGB(r*r - d, 0, 0);
    } else {
      leds[i] = CRGB::Black;
    }
  }
}

void draw_snow(int count) 
{
  for (int i = 0; i < count; i++) {
    int j = random(0, 600);
    leds[j] = CHSV(random(0, 128), 0, 10);
  }
}

void imu_circles() 
{
  t += 0.02;
  
  //y = 7.0;
  x += angle_diff / 360.0 * width;
  
  c = (1.0+sin(t))*128;
  s = 1.0; //(1.0+sin(t*20)) / 2.0;

  // wrap x coordinates
  if (x > width)  x -= width;
  if (x < -width) x += width;
}

void bouncing_circles() 
{
  t += 0.02;
  
  y += vy;
  x += vx;
  
  c = (1.0+sin(t))*128;
  s = 1.0; //(1.0+sin(t*20)) / 2.0;

  if (y < 0+4 || y > 17+4) vy = -vy;

  // wrap x coordinates
  if (x > width)  x -= width;
  if (x < -width) x += width;
}


void loop() 
{
  button.loop();
  calculate_angle();
  
  int counter = (int)encoder.getCount();
  if(counter < 0) {
    counter = 0;
    encoder.setCount(counter);
  } else if(counter > 10) {
    counter = 10;
    encoder.setCount(counter);
  }
  //Serial.println(counter);

  int state = button.getCount() % 2;

  switch(state) {
    case 0: imu_circles(); break;
    case 1: bouncing_circles(); break;
  }
  
  draw_circles();
  draw_snow(counter*10);
  
  FastLED.show();
  FastLED.delay(10);
}
