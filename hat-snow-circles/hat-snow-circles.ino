// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library


#include <ezButton.h>  // the library to use for SW pin

#define CLK_PIN 5  // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  18 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  19 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin

#define DIRECTION_CW  0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

volatile int counter = 0;
volatile int direction = DIRECTION_CW;
volatile unsigned long last_time;  // for debouncing
int prev_counter;

ezButton button(SW_PIN);  // create ezButton object that attach to pin 7;


int random_dots = 0;


#include <FastLED.h>
#define NUMPIXELS 600 // Number of LEDs in strip

// Here's how to control the LEDs from any two pins:
#define DATAPIN    13

#define NUMX 33.5
const float px_dist = 1.6;
const float px_per_cm = 34.5;
const float width = px_per_cm*px_dist;


CRGB leds[NUMPIXELS];

struct Point {
  float h;
  float a;
};

Point coords[NUMPIXELS];



void IRAM_ATTR ISR_encoder() 
{
  //if ((millis() - last_time) < 10)  // debounce time is 50ms
  //  return;

  if (digitalRead(DT_PIN) == HIGH) {
    // the encoder is rotating in counter-clockwise direction => decrease the counter
    counter--;
    direction = DIRECTION_CCW;
  } else {
    // the encoder is rotating in clockwise direction => increase the counter
    counter++;
    direction = DIRECTION_CW;
  }

  // limit the counter
  if (counter < 0) {
    counter = 0;
  } else if(counter > 10) {
    counter = 10;
  }

  last_time = millis();
}


void setup() 
{
  Serial.begin(9600);

  // configure encoder pins as inputs
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  button.setDebounceTime(50);  // set debounce time to 50 milliseconds

    
    
  // use interrupt for CLK pin is enough
  // call ISR_encoder() when CLK pin changes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(CLK_PIN), ISR_encoder, RISING);
  
    
  FastLED.addLeds<WS2812B, DATAPIN, GRB>(leds, NUMPIXELS);

  float h = 0;
  float a = 0;
  coords[0].h = h;
  coords[0].a = a;
  
  for (int i = 1; i < NUMPIXELS; ++i) {
    h += 1.2 / px_per_cm;
    a += px_dist;
    
    if(a > width) {
      a -= width;
    }

    coords[i].h = h;
    coords[i].a = a;
  }
}

int idx = 0;
float t = 0.0;
float x = 8.0;
float y = 8.0;
float c = 0.0;

float vx = 0.5;
float vy = 0.1;

float s = 0;

float checkCircle(const Point& c, float x, float y) 
{
  float dx = (c.a - x);
  if (abs(dx) > width) dx -= (int)(dx/width)*width;
  
  if (dx > width/2) dx -= width;
  else if (dx < -width/2) dx += width;
  
  float dy = (c.h - y);
  
  float d = dx*dx + dy*dy;  

  return d;
}

void getButtons() {
    button.loop();  // MUST call the loop() function first
    
   if (prev_counter != counter) {
    Serial.print("Rotary Encoder:: direction: ");
    if (direction == DIRECTION_CW)
      Serial.print("CLOCKWISE");
    else
      Serial.print("ANTICLOCKWISE");

    Serial.print(" - count: ");
    Serial.println(counter);

    prev_counter = counter;
  }

  if (button.isPressed()) {
    Serial.println("The button is pressed");
  }
}

void loop() 
{
  getButtons();
 
  float r = 5.0;
  for (int i = 0; i < NUMPIXELS; ++i) 
  {
    float d  = checkCircle(coords[i], x         ,y);
    float d1 = checkCircle(coords[i], x+width/2 ,y);
    if(d < r*r) {
      leds[i] = CHSV(c, 255, 100-(d/(r*r))*100*s); //CRGB(r*r - d, 0, 0);
    } else if(d1 < r*r) {
      leds[i] = CHSV(c, 255, 100-(d1/(r*r))*100*s); //CRGB(r*r - d, 0, 0);
    } else {
      leds[i] = CRGB::Black;
    }
  }

  for (int i = 0; i < counter*10; i++) {
    int j = random(0, 600);
    leds[j] = CHSV(random(0, 128), 0, 10);
  }

  // (5.0*5.0 - d)*10
  //c-(5.0*5.0 - d)*5
  FastLED.show();
  
  //idx = (idx + 1) % 36;

  t += 0.02;
  //y = 3.0 * sin(t) + 8.0;
  //x = 3.0 * cos(t) + 8.0;
  y += vy;
  x += vx;
  c = (1.0+sin(t))*128;
  s = (1.0+sin(t*10));

  if (y < 0+4 || y > 17+4) vy = -vy;
  if (x > width) x -= width;
  if (x < -width) x += width;
  
  delay(10);
}
