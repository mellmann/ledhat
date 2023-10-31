// https://github.com/madhephaestus/ESP32Encoder/blob/master/src/InterruptEncoder.cpp

#include "Encoder.h"

Encoder::EncoderState Encoder::state;

Encoder::Encoder(int clk, int dt)
{
  state.clk = clk;
  state.dt = dt;
  
  pinMode(clk, INPUT_PULLUP);
  pinMode(dt, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(clk), ISR_encoder, RISING);
}

void IRAM_ATTR Encoder::ISR_encoder() 
{
  long start = micros();
	long duration = start - state.microsLastA;
	
  if (duration >= US_DEBOUNCE) 
  {
		state.microsLastA = start;
		state.microsTimeBetweenTicks = duration;
    
		state.aState = digitalRead(state.clk);
		state.bState = digitalRead(state.dt);
    
		if (state.bState == HIGH/*state.bState*/)
			state.count++;
		else
			state.count--;
	}
}
