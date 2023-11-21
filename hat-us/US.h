// inspiration from
// https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino

#ifndef US_H
#define US_H

template<class MATRIX_TYPE>
class US
{
public:
  US(MATRIX_TYPE& matrix) : matrix(matrix) {
  }

  void update(int factor) 
  {
    
    for( int x = 0; x < matrix.width(); ++x) {
      for( int y = 0; y < matrix.height(); ++y) {
        //matrix.set_pixel(x, (matrix.height()-1) - y, HeatColor( heat[x][y]));
        int i = (x/3) % 2;
        if (i == 0) {
          matrix.pixel(x, y) = CRGB(64, 0, 0); // 
        } else {
          matrix.pixel(x, y) = CRGB(64, 64, 64); // 
        }
      }
    }

    for( int x = 0; x < matrix.width(); ++x) {
      matrix.pixel(x, 0) = CRGB(0, 0, 64);
      if((x+1) % 4 == 0) {
        matrix.pixel(x, 1) = CRGB(64, 64, 64);
      } else {
        matrix.pixel(x, 1) = CRGB(0, 0, 64);
      }
      matrix.pixel(x, 2) = CRGB(0, 0, 64);
    }
  }

private:
  MATRIX_TYPE& matrix;

};

#endif // US_H
