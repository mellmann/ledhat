
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

#define NUMPIXELS     600 // number of LEDs in strip
#define MATRIX_WIDTH  69  // number of pixes in a line of the virtual matrix
#define MATRIX_HEIGHT 18  // number of line in the virtual matrix

/*
 * // this might make the code more elegant, but also slower because of virtual functions
class SimplePixelMatrix 
{
public:
  virtual int width() = 0;
  virtual int height() = 0;
  virtual CRGB& pixel(uint8_t x, uint8_t y) = 0;
  virtual void set_pixel(uint8_t x, uint8_t y, const CRGB& color) = 0;
}
*/


template<int NUM_PIXELS, int WIDTH, int HEIGHT>
class HatMatrix
{
public:
  HatMatrix()
  {
    // initialize the mapping of the matrix values to the led strip
    for(int i = 0; i < WIDTH * HEIGHT; ++i) {
      xy2idx[i] = (i % 2 == 0) ? min(i / 2, NUM_PIXELS) : NUM_PIXELS;
    }
  }

  inline CRGB& pixel(uint8_t x, uint8_t y) {
    return leds[XY(x,y)];
  }
  
  inline void set_pixel(uint8_t x, uint8_t y, const CRGB& color) {
    leds[XY(x,y)] = color;
  }

  inline CRGB* getLEDArray() {
    return leds;
  }

  inline int num_pixels() const {
    return NUM_PIXELS;
  }

  inline int width() const {
    return WIDTH;
  }
  
  inline int height() const {
    return HEIGHT;
  }

private:
  uint16_t XY(uint8_t x, uint8_t y)
  {
    // any out of bounds address maps to the first hidden pixel
    if ( (x >= WIDTH) || (y >= HEIGHT) ) {
      return NUM_PIXELS;
    }
  
    return xy2idx[y * WIDTH + x];
  }

public:
  static const uint8_t Width  = WIDTH;
  static const uint8_t Height = HEIGHT;
  static const int Num_Pixels = NUM_PIXELS;

private:
  // the last pixel is invalid
  CRGB leds[ NUM_PIXELS + 1 ];

  // maps y*kMatrixWidth + x to LED-idx
  uint16_t xy2idx[WIDTH * HEIGHT];
};

typedef HatMatrix<NUMPIXELS, MATRIX_WIDTH, MATRIX_HEIGHT> MyHatMatrix;
MyHatMatrix matrix;



void setup() 
{  
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

  // register the LED array
  FastLED.addLeds<WS2812B, DATAPIN, GRB>(matrix.getLEDArray(), matrix.num_pixels());
  FastLED.setBrightness(64);
  FastLED.setDither(0);
}

template<class MATRIX_TYPE>
class Fire
{
public:
  Fire(MATRIX_TYPE& matrix) : matrix(matrix) {}

  void update(int factor) 
  {
    int COOLING  = cooling_base  + factor*10;
    int SPARKING = sparkling_base - factor*10;
    
    // Step 1.  Cool down every cell a little
    for( int x = 0; x < matrix.width(); ++x) {
      for( int y = 0; y < matrix.height(); ++y) {
        heat[x][y] = qsub8( heat[x][y],  random8(0, ((COOLING * 10) / matrix.height()) + 2));
      }
    }
      
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int x = 0; x < matrix.width(); ++x) {
      for( int y = matrix.height()-1; y >= 2; y--) {
        heat[x][y] = (heat[x][y - 1] + heat[x][y - 2] + heat[x][y - 2] ) / 3;
      } 
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    for( int x = 0; x < matrix.width(); ++x) {
      if( random8() < SPARKING ) {
        int y = random8(2);
        heat[x][y] = qadd8( heat[x][y], random8(160,255) );
      }
    }
  
    // Step 4.  Map from heat cells to LED colors
    for( int x = 0; x < matrix.width(); ++x) {
      for( int y = 0; y < matrix.height(); ++y) {
        //matrix.set_pixel(x, (matrix.height()-1) - y, HeatColor( heat[x][y]));
        matrix.pixel(x, (matrix.height()-1) - y) = HeatColor( heat[x][y]);
      }
    }
  }

private:
  MATRIX_TYPE& matrix;
  
  int cooling_base   = 100;
  int sparkling_base = 120;
  
  // array of temperature readings at each simulation cell
  uint8_t heat[MATRIX_TYPE::Width][MATRIX_TYPE::Height];
};

Fire<MyHatMatrix> fire(matrix);



void loop() 
{
  const int maxCounter = 10;
  int counter = encoder.getCount();
  if(counter < 0) {
    counter = 0;
    encoder.setCount(counter);
  } else if(counter >= maxCounter) {
    counter = maxCounter;
    encoder.setCount(counter);
  }
  //Serial.println(counter);

  FastLED.clear();

  fire.update(counter);

  //counter = (counter + 1) % kMatrixHeight;
  FastLED.show();
  FastLED.delay(25);
}
