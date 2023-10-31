// https://github.com/madhephaestus/ESP32Encoder/blob/master/src/InterruptEncoder.cpp

#ifndef ENCODER_H
#define ENCODER_H

#define US_DEBOUNCE 1000

#include <Arduino.h>

class Encoder 
{
  
public:
  Encoder(int clk, int dt);

  int64_t getCount() {
    return state.count;
  }
  
  void setCount(int64_t count) {
    state.count = count;
  }
  

public:

  class EncoderState {
    public:
      int clk;
      int dt;
  
      volatile bool aState=0;
      volatile bool bState=0;
      volatile int64_t count=0;
      volatile int64_t microsLastA=0;
      volatile int64_t microsTimeBetweenTicks=0;
  };
 
public:
  static void IRAM_ATTR ISR_encoder();
 
public:
  static EncoderState state;
};


#endif // ENCODER_H
