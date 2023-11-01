// https://github.com/madhephaestus/ESP32Encoder/blob/master/src/InterruptEncoder.cpp

#include "Encoder.h"

Encoder::EncoderState Encoder::state;

Encoder::Encoder(int clk, int dt)
{
  state.clk = clk;
  state.dt = dt;
  
  pinMode(clk, INPUT_PULLUP);
  pinMode(dt, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(clk), ISR_encoder, CHANGE);
}

void IRAM_ATTR Encoder::ISR_encoder() 
{
  long start = micros();
	long duration = start - state.microsLastA;
	
  if (duration >= US_DEBOUNCE) 
  {
    // NOTE: we are listening to changes in slk, 
    // so we know that clk changed at this point
    
    // read new data
		bool aState = digitalRead(state.clk);
		bool bState = digitalRead(state.dt);
    
		if (aState == bState) {
			state.count++;
		} else {
			state.count--;
		}
	}
}
