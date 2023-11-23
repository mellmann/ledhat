// inspiration from
// https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino

#ifndef FIRE_H
#define FIRE_H

template<class MATRIX_TYPE>
class Fire
{
public:
  Fire(MATRIX_TYPE& matrix) : matrix(matrix) {
    color_palette = HeatColors_p;
  }

  void update(int factor) 
  {
    int COOLING  = cooling_base   + factor*10;
    int SPARKING = sparkling_base - factor*10;
    
    // Step 1.  Cool down every cell a little
    for( int x = 0; x < matrix.width(); ++x) {
      for( int y = 0; y < matrix.height(); ++y) {
        heat[x][y] = qsub8( heat[x][y],  random8(0, ((COOLING * 10) / matrix.height()) + 2));
      }
    }
      
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int y = matrix.height()-1; y >= 2; --y) {
      for( int x = 0; x < matrix.width(); ++x) {
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
        //matrix.pixel(x, (matrix.height()-1) - y) = HeatColor( heat[x][y]); // 

        uint8_t colorindex = scale8( heat[x][y], 240);
        matrix.pixel(x, y) = ColorFromPalette(color_palette, colorindex);
      }
    }
  }

private:
  MATRIX_TYPE& matrix;

  CRGBPalette16 color_palette;
  
  int cooling_base   = 100;
  int sparkling_base = 120;
  
  // array of temperature readings at each simulation cell
  uint8_t heat[MATRIX_TYPE::Width][MATRIX_TYPE::Height];
};

#endif // FIRE_H
