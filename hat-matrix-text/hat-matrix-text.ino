
// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Arduino.h>

#include "Net.h"
Net net;

#include "WebServer.h"
WebServer web_server(3001);


#include <Adafruit_GFX.h>
#include "HatMatrix.h"

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


#include "LEDMatrix.h"

#include "LEDText.h"
#include "FontRobotron.h"
#include "FontMatrise.h"
#include "FontAtari.h"

#include "Ball.h"
#include "Fire.h"

#define NUMPIXELS 600 // Number of LEDs in strip
#define MATRIX_WIDTH   69
#define MATRIX_HEIGHT  18
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX


cLEDMatrix<MATRIX_WIDTH, MATRIX_HEIGHT, MATRIX_TYPE> ledsm;
cLEDText ScrollingMsg;
const unsigned char TxtRainbowL[] = { EFFECT_HSV "\x00\xff\xff" "H" EFFECT_HSV "\x20\xff\xff" "+" EFFECT_HSV "\x40\xff\xff" "V" EFFECT_HSV "\x60\xff\xff" "=" EFFECT_HSV "\x80\xff\xff" "<3"};
//const unsigned char TxtRainbowL[] = { EFFECT_RGB "\x00\x00\xff" "Max Test" EFFECT_RGB "\xff\xff\xff" };

// int idx = 4 + i * 5;
unsigned char TxtMessage[] = { 
//     0       1    2   3    4
  EFFECT_HSV "\x00\xff\xff" "M" // 0
  EFFECT_HSV "\x20\xff\xff" "E" // 1 
  EFFECT_HSV "\x40\xff\xff" "S" // 2 
  EFFECT_HSV "\x60\xff\xff" "S" // 3 
  EFFECT_HSV "\x80\xff\xff" "A" // 4
  EFFECT_HSV "\xA0\xff\xff" "G" // 5 
  EFFECT_HSV "\xC0\xff\xff" "E" // 6 
  EFFECT_HSV "\xD0\xff\xff" "!" // 7 
};

class GFXTest : public GFXcanvas1 
{
public:
 HatMatrix<NUMPIXELS, MATRIX_WIDTH, MATRIX_HEIGHT> hatMatrix;

public:
  GFXTest()
    : GFXcanvas1(MATRIX_WIDTH, MATRIX_HEIGHT) 
  {
  }

  void print(bool rotated) 
  {
    //Serial.println("print");
    char pixel_buffer[8];
    uint16_t width, height;
  
    if (rotated) {
      width = this->width();
      height = this->height();
    } else {
      width = this->WIDTH;
      height = this->HEIGHT;
    }
  
    for (uint16_t y = 0; y < height; y++) {
      for (uint16_t x = 0; x < width; x++) {
        bool pixel;
        if (rotated) {
          pixel = this->getPixel(x, y);
        } else {
          pixel = this->getRawPixel(x, y);
        }
        //sprintf(pixel_buffer, " %d", pixel);
        //Serial.print(pixel_buffer);
        if(pixel) {
          hatMatrix.pixel(x,y) = CRGB::Red;
        } else {
          hatMatrix.pixel(x,y) = CRGB::Black;
        }
      }
    }
  }
};


GFXTest gfx;



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

// fire animation
Fire<MyHatMatrix> fire(matrix);



void setup() 
{  
  Serial.begin(115200);
  while (!Serial) {};

  //encoder.attachFullQuad ( DT_PIN, CLK_PIN );
  encoder.attachSingleEdge ( DT_PIN, CLK_PIN );
  encoder.setCount ( 0 );

  button.setDebounceTime(50);  // set debounce time to 50 milliseconds

  

  Serial.println("Initializing IMU...");
  if(!bno.begin()) {
      Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
  } else {
    Serial.println("BNO055 connected!");
    delay(1000); // FIXME: do we need that?
    bno.setExtCrystalUse(true);
  }
  
  //FastLED.addLeds<WS2812B, DATAPIN, GRB>(leds, NUMPIXELS);
  //FastLED.addLeds<WS2812B, DATAPIN, GRB>(gfx.hatMatrix.getLEDArray(), gfx.hatMatrix.num_pixels());
  FastLED.addLeds<WS2812B, DATAPIN, GRB>(matrix.getLEDArray(), matrix.num_pixels());
  FastLED.setBrightness(255);
  FastLED.setDither(0);


  ScrollingMsg.SetFont(RobotronFontData);
  //ScrollingMsg.SetFont(AtariFontData);
  
  //ScrollingMsg.SetFont(MatriseFontData);
  ScrollingMsg.Init(&ledsm, ledsm.Width(), ScrollingMsg.FontHeight() + 1, 0, 5);
  //ScrollingMsg.SetText((unsigned char *)TxtRainbowL, sizeof(TxtRainbowL) - 1);
  ScrollingMsg.SetText((unsigned char *)TxtMessage, sizeof(TxtMessage) - 1);
  ScrollingMsg.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
  ScrollingMsg.UpdateText();

  net.connect("emf2024-open", "");
  web_server.begin();
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



int text_position_x = 0;
unsigned int time_of_last_message_displayed = 0;

unsigned int time_of_last_message_pull = 0;

#define MESSAGE_PULL_TIME 30000

void set_text(String message) 
{
  for(int i = 0; i < 8; ++i) 
  {
    int idx = 4 + i * 5;
    if(i < message.length()) {
      TxtMessage[idx] = message.charAt(i);
    } else {
      TxtMessage[idx] = ' ';
    }
  }

  ScrollingMsg.SetText((unsigned char *)TxtMessage, sizeof(TxtMessage) - 1);
  ScrollingMsg.UpdateText();
}

void update_text() 
{
  // try to pull new message
  if(time_of_last_message_pull == 0 || millis() > time_of_last_message_pull + MESSAGE_PULL_TIME) {
    web_server.pull_new_message();
    time_of_last_message_pull = millis();
  }
  
  if(web_server.time_of_last_message != 0 && web_server.time_of_last_message > time_of_last_message_displayed) 
  {
      Serial.print("Update Message: ");
      Serial.println(web_server.message);

      set_text(web_server.message);

      time_of_last_message_displayed = web_server.time_of_last_message;
  }
 
  // copy data
  for( int x = 0; x < matrix.width(); ++x) {
    for( int y = 0; y < matrix.height(); ++y) {
      const int x_rotated = (x + text_position_x) % matrix.width();
      matrix.pixel( (matrix.width()-1) - x_rotated, y) = ledsm(x,y);
    }
  }

  text_position_x = text_position_x - 1;
  if (text_position_x < 0) { 
    text_position_x += matrix.width();
  }
}


int getEncoder() 
{
  //const int maxCounter = 18;
  const int maxCounter = 25;
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


int button_count_old = 0;
int app_state = 0;
unsigned long app_state_last_change_time = 0;

void loop() 
{
  button.loop();
  calculate_angle();
  int counter = getEncoder();
  
  unsigned long app_state_time = millis() - app_state_last_change_time;
  

  int button_count = button.getCount();
  if((app_state_time > 1000 && button_count != button_count_old) || app_state_time > 10000) 
  {
    app_state = (app_state + 1) % 4;
    app_state_last_change_time = millis();
  }
  button_count_old = button_count;

  // there is a new message
  if(millis() < time_of_last_message_displayed + MESSAGE_PULL_TIME + 1000) {
    app_state = 3;
  }
  
  switch(app_state) 
  {
    case 0: 
      FastLED.clear();
      FastLED.setBrightness(255);
      counter = 10;
      spaceBalls.draw_snow(counter*10); 
      spaceBalls.update_imu(angle_diff); 
      break;
    case 1:
      FastLED.clear();
      FastLED.setBrightness(255);
      counter = 10;
      spaceBalls.draw_snow(counter*10); 
      spaceBalls.update_bouncing();
      break;
    case 2: 
      FastLED.clear();
      FastLED.setBrightness(64);
      fire.update(counter);
      break;
    case 3:
      FastLED.clear();
      FastLED.setBrightness(counter*10+5);
      update_text();
      break;
  }

  /*
  FastLED.clear();
  gfx.drawRect(7,7,6,6,8);
  gfx.println("A");
  gfx.print(false);
  */
  
  FastLED.show();
  FastLED.delay(25);
}
