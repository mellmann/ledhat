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
#define NUMPIXELS     600 // number of LEDs in strip
#define MATRIX_WIDTH  69  // number of pixes in a line of the virtual matrix
#define MATRIX_HEIGHT 18  // number of line in the virtual matrix

// Here's how to control the LEDs from any two pins:
#define DATAPIN    13
//#define MAX_POWER_MILLIAMPS 500

//Encoder encoder(DT_PIN, CLK_PIN);
ESP32Encoder encoder;
ezButton button(SW_PIN);
Adafruit_BNO055 bno;

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
const float px_dist_x = 8;  // mm
const float px_dist_y = 12; // mm

CRGB leds[NUMPIXELS];

struct Point {
  float x;
  float y;
};

struct Pixel {
  uint16_t x;
  uint16_t y;
};

Pixel coords_px[NUMPIXELS];
Point coords_mm[NUMPIXELS];

void calculate_coords()
{
  uint16_t x = 0;
  uint16_t y = 0;

  // count from the back, to ensure the filst row is at the bottom of the hat
  for (int i = NUMPIXELS-1; i >= 0 ; --i) {
    coords_px[i] = {x, y};
    coords_mm[i] = {x*px_dist_x, y*px_dist_y};
    
    x += 2;
    if(x >= MATRIX_WIDTH) {
      x -= MATRIX_WIDTH;
      y++;
    }
  }

  for (int i = 0; i < NUMPIXELS; ++i) {
    Serial.println(String("") + i + ": " + coords_px[i].x + ", " + coords_px[i].y);
  }
}

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

  FastLED.addLeds<WS2812B, DATAPIN, GRB>(leds, NUMPIXELS);
  FastLED.setBrightness(255);
  FastLED.setDither(BINARY_DITHER);
  //FastLED.setDither(DISABLE_DITHER);
  //FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);

  calculate_coords();
  generate_balls();
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

float width  = MATRIX_WIDTH*px_dist_x;
float height = MATRIX_HEIGHT*px_dist_y;

class Ball 
{
public:
  Ball() 
    : x(0.0), y(0.0), r(0.0)
  {}
  
  Ball(float x, float y, float r) 
    : x(x), y(y), r(r)
  {
    vx = 10.0 * r / 50.0;
    vy = (10.0 - vx) * r / 50.0;

    max_brightness = 255.0*r / 50.0;

    t = r / 50.0 * 3.14;
  }

  void update_imu(float angle_diff) 
  {
    t += 0.02;
    
    x -= angle_diff / 360.0 * width;
    //y = 7.0;
    
    c = (1.0+sin(t))*128;
    s = 1.0; //(1.0+sin(t*20)) / 2.0;
  
    // wrap x coordinates
    if (x > width)  {
      x -= width;
    }
    if (x < -width) {
      x += width;
    }
  }

  void update_bouncing() 
  {
    t += 0.02;
  
    y += vy;
    x += vx;
    
    c = (1.0+sin(t))*128;
    s = 1.0; //(1.0+sin(t*20)) / 2.0;
  
    if (y < 0 + 50 || y > height - 50) {
      vy = -vy;
    }
  
    // wrap x coordinates
    if (x > width)  {
      x -= width;
    }
    if (x < -width) {
      x += width;
    }
  }
  
  void draw() 
  {
    for (int i = 0; i < NUMPIXELS; ++i) 
    {
      float d  = dist(coords_mm[i], x ,y);
      if(d < r) {
        leds[i] = CHSV(c, 255, (1.0-d/r)*(1.0-d/r)*max_brightness);
      }
    }
  }

private:
  float dist(const Point& p, float x, float y) 
  {
    float dx = p.x - x;
  
    // wrap around
    if (abs(dx) > width) { 
      dx -= (int)(dx/width)*width;
    }
    
    if (dx > width/2) {
      dx -= width;
    } else if (dx < -width/2) {
      dx += width;
    }
    
    float dy = p.y - y;
    
    float d = dx*dx + dy*dy;  
  
    return sqrt(d);
  }
  
private:
  float x;
  float y;
  float r;

private:
  float vx = 8.0;
  float vy = 2.0;

private:
  float t = 0.0;
  float c = 0.0;
  float s = 0.0;
  float max_brightness = 255;
};


#define NUMBER_OF_BALLS 12
Ball balls[NUMBER_OF_BALLS];

void generate_balls() 
{
  for (int i = 0; i < NUMBER_OF_BALLS; i++) 
  {
    int x = random(0, MATRIX_WIDTH);
    int y = random(0, MATRIX_HEIGHT);
    int r = (1 + i / 4) * 16; //random(15, 50);
    balls[i] = Ball(x*px_dist_x, y*px_dist_y, r);
  }
}

void draw_snow(int count) 
{
  for (int i = 0; i < count; i++) {
    int j = random(0, 600);
    leds[j] = CHSV(random(0, 128), 0, 10);
  }
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
  
  int counter = getEncoder() ;

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
  

  for (Ball& ball: balls)
  {
    switch(state) {
      case 0: ball.update_imu(angle_diff); break;
      case 1: ball.update_bouncing(); break;
    }
    
    ball.draw();
  }
  
  draw_snow(counter*10);
  
  FastLED.show();
  FastLED.delay(10);
}
