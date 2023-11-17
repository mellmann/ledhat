
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

#endif //HAT_MATRIX_H
