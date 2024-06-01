
#include <FastLED.h>
#include "HatMatrix.h"

#ifndef BALL_H
#define BALL_H

template<class MATRIX_TYPE>
class Ball 
{
public:
  Ball(): x(0.0), y(0.0), r(0.0)
  {}
  
  Ball(float x, float y, float r) 
    : x(x), y(y), r(r)
  {
    vx = 10.0 * r / 50.0;
    vy = (10.0 - vx) * r / 50.0;

    max_brightness = 255.0*r / 50.0;

    t = r / 50.0 * 3.14;
  }

  void update_imu(MATRIX_TYPE& matrix, float angle_diff) 
  {
    t += 0.02;

    // radial coordinates
    x -= angle_diff / 360.0 * matrix.width_mm;
    //y = 7.0;
    
    c = (1.0+sin(t))*128;
    s = 1.0; //(1.0+sin(t*20)) / 2.0;

    x = matrix.wrap_x(x);
  }

  void update_bouncing(MATRIX_TYPE& matrix) 
  {
    t += 0.02;
  
    y += vy;
    x += vx;
    
    c = (1.0+sin(t))*128;
    s = 1.0; //(1.0+sin(t*20)) / 2.0;

    // bounce off the borders
    if (y < 0 + r || y > matrix.height_mm - r) {
      vy = -vy;
    }

    x = matrix.wrap_x(x);
  }

  void draw(MATRIX_TYPE& matrix) 
  {
    for (int i = 0; i < matrix.num_pixels(); ++i) 
    {
      float d  = matrix.dist(matrix.coords_mm[i], x ,y);
      if(d < r) {
        matrix.getLED(i) = CHSV(c, 255, (1.0-d/r)*(1.0-d/r)*max_brightness);
      }
    }
  }

  void draw_circles(MATRIX_TYPE& matrix) 
  {
    for (int i = 0; i < matrix.num_pixels(); ++i) 
    {
      float d  = matrix.dist(matrix.coords_mm[i], x ,y);
      if(d < r) {
        matrix.getLED(i) = CHSV(c, 255, (d/r)*max_brightness);
      }
    }
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

template<class MATRIX_TYPE>
class SpaceBalls 
{
public:
  SpaceBalls(MATRIX_TYPE& matrix) : matrix(matrix) 
  {
    for (int i = 0; i < NUMBER_OF_BALLS; i++) 
    {
      int x = random(0, matrix.width());
      int y = random(0, matrix.height());
      int r = (1 + i / 4) * 16; //random(15, 50);
      balls[i] = Ball<MATRIX_TYPE>(x*matrix.px_dist_x, y*matrix.px_dist_y, r);
    }  
  }

  void update_imu(float angle_diff) {
    for (Ball<MATRIX_TYPE>& ball: balls)
    {
      ball.update_imu(matrix, angle_diff);
      ball.draw(matrix);
    }
  }

  void update_bouncing() {
    for (Ball<MATRIX_TYPE>& ball: balls)
    {
      ball.update_bouncing(matrix);
      ball.draw(matrix);
    }
  }
  
  void draw_snow(int count) 
  {
    for (int i = 0; i < count; i++) {
      int j = random(0, matrix.num_pixels());
      matrix.getLED(j) = CHSV(random(0, 128), 0, 10);
    }
  }
  
private:
  MATRIX_TYPE& matrix;
  
  Ball<MATRIX_TYPE> balls[NUMBER_OF_BALLS];
};

#endif //BALL_H
