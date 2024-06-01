
#include <FastLED.h>

#ifndef HAT_MATRIX_H
#define HAT_MATRIX_H

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

struct Point {
  float x;
  float y;
};

struct Pixel {
  uint16_t x;
  uint16_t y;
};

template<int NUM_PIXELS, int WIDTH, int HEIGHT>
class HatMatrix
{
public:
  HatMatrix(float px_dist_x = 1.0, float px_dist_y = 1.0)
    : px_dist_x(px_dist_x), 
      px_dist_y(px_dist_y),
      width_mm(WIDTH*px_dist_x),
      height_mm(HEIGHT*px_dist_y)
  {
    // initialize the mapping of the matrix values to the led strip
    for(int i = 0; i < WIDTH * HEIGHT; ++i) {
      xy2idx[i] = (i % 2 == 0) ? min((NUM_PIXELS - 1) - i / 2, NUM_PIXELS) : NUM_PIXELS;
    }

    //
    calculate_coords();
  }

  inline CRGB& pixel(uint8_t x, uint8_t y) {
    return leds[XY(x,y)];
  }
  
  inline void set_pixel(uint8_t x, uint8_t y, const CRGB& color) {
    leds[XY(x,y)] = color;
  }

  inline CRGB& getLED(int i) {
    return leds[i];
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

  // cylindrical geometry
  inline int wrap_x(int x) 
  {
    // wrap x coordinates
    if (x > width_mm)  {
      x -= width_mm;
    }
    if (x < -width_mm) {
      x += width_mm;
    }
    return x;
  }

  float dist(const Point& p, float x, float y) 
  {
    float dx = p.x - x;
  
    // wrap around
    if (abs(dx) > width_mm) { 
      dx -= (int)(dx/width_mm)*width_mm;
    }
    
    if (dx > width_mm/2) {
      dx -= width_mm;
    } else if (dx < -width_mm/2) {
      dx += width_mm;
    }
    
    float dy = p.y - y;
    
    float d = dx*dx + dy*dy;  
  
    return sqrt(d);
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

  void calculate_coords()
  {
    uint16_t x = 0;
    uint16_t y = 0;
  
    // count from the back, to ensure the filst row is at the bottom of the hat
    for (int i = NUM_PIXELS-1; i >= 0 ; --i) {
      coords_px[i] = {x, y};
      coords_mm[i] = {x*px_dist_x, y*px_dist_y};
      
      x += 2;
      if(x >= WIDTH) {
        x -= WIDTH;
        y++;
      }
    }
    
    //for (int i = 0; i < NUM_PIXELS; ++i) {
    //  Serial.println(String("") + i + ": " + coords_px[i].x + ", " + coords_px[i].y);
    //}
  }

public:
  static const uint8_t Width  = WIDTH;
  static const uint8_t Height = HEIGHT;

  static const int Num_Pixels = NUM_PIXELS;

  const float px_dist_x;  // mm
  const float px_dist_y; // mm
  const float width_mm; // mm
  const float height_mm; // mm

  Pixel coords_px[NUM_PIXELS];
  Point coords_mm[NUM_PIXELS];

private:
  // the last pixel is invalid
  CRGB leds[ NUM_PIXELS + 1 ];

  // maps y*kMatrixWidth + x to LED-idx
  uint16_t xy2idx[WIDTH * HEIGHT];
};

#endif //HAT_MATRIX_H
