#include "FastLED.h"

#define NUM_LEDS             300
#define DATA_PIN             2
#define BRIGHTNESS           150
#define LED_COLOR_ORDER      GRB

CRGB leds[NUM_LEDS];
int brigtness_weak = 16;
int brigtness_normal = 128;
int brigtness_high = 255;

const int berlin_offset = 254 + 17;

// Initialisierung benötigter Variablen
int Counter = 0; 
int CounterHome = 0;//berlin_offset*2; //
int CounterPos = 0;
boolean Richtung;
int Pin_clk_Letzter;  
int Pin_clk_Aktuell;
 
// Definition der Eingangs-Pins
int pin_clk = 3;
int pin_dt = 4; 
int button_pin = 5; 

int dist[NUM_LEDS];

unsigned int last_time_of_wave = 0;
 

void setup_counter() {
  pinMode (pin_clk,INPUT);
  pinMode (pin_dt,INPUT);
  pinMode (button_pin,INPUT);
  
  // ...und deren Pull-Up Widerstände aktiviert
  digitalWrite(pin_clk, true);
  digitalWrite(pin_dt, true);
  digitalWrite(button_pin, true);
  
  // Initiales Auslesen des Pin_CLK
  Pin_clk_Letzter = digitalRead(pin_clk);


  Counter = CounterHome;
}

// hack
bool do_animate_wave = false;
int switch_time = 0;

void update_counter()
{
  Pin_clk_Aktuell = digitalRead(pin_clk);
  
  // check the change
  if (Pin_clk_Aktuell != Pin_clk_Letzter)
  { 
    if (digitalRead(pin_dt) != Pin_clk_Aktuell) {  
      Counter ++;
      Richtung = true;
    } else {
      Richtung = false;
      Counter--;
    }
    Counter = max(Counter, 0);
  } 
  
  Pin_clk_Letzter = Pin_clk_Aktuell;
  
  if (!digitalRead(button_pin)){
    Counter = CounterHome;

    if(switch_time > 20) {
      do_animate_wave = !do_animate_wave;
      switch_time = 0;
    }
  }

  if(Counter % 2 == 0) {
    CounterPos = Counter/2;
  }

  switch_time = min(switch_time+1,10000);
}

int last_counter = -1;

void setup_dist()
{
  for(int i = 0; i < NUM_LEDS; ++i) {
    dist[i] = 1000;
  }

dist[16] = 375;
dist[15] = 372;
dist[14] = 370;
dist[13] = 370;
dist[12] = 365;
dist[11] = 353;
dist[10] = 355;
dist[9] = 350;
dist[8] = 375;
dist[7] = 382;
dist[6] = 390;
dist[5] = 398;
dist[4] = 400;
dist[3] = 424;
dist[2] = 428;
dist[1] = 430;
dist[0] = 443;


int* north = &(dist[17]);
  
north[254] = 0;
north[255] = 43;
north[256] = 55;
north[257] = 73;
north[258] = 88;
north[259] = 105;
north[260] = 120;
north[261] = 118;
north[262] = 99;
north[263] = 74;
north[264] = 63;
north[265] = 50;
north[266] = 75;
north[267] = 68;
north[268] = 50;
north[269] = 62;
north[270] = 73;
north[271] = 92;
north[272] = 100;

north[253] = 18;
north[252] = 33;
north[251] = 50;
north[250] = 127;
north[249] = 143;
north[248] = 163;
north[247] = 154;
north[246] = 130;
north[245] = 133;
north[244] = 104;
north[243] = 87;
north[242] = 72;
north[241] = 93;
north[240] = 108;
north[239] = 130;
north[238] = 101;
north[237] = 112;
north[236] = 146;
north[235] = 120;
north[234] = 101;
north[233] = 84;
north[232] = 69;
north[231] = 52;
north[230] = 32;
north[229] = 18;
north[228] = 13;
north[227] = 21;
north[226] = 38;
north[225] = 55;
north[224] = 72;
north[223] = 90;
north[222] = 150;
north[221] = 125;
north[220] = 140;
north[219] = 157;
north[218] = 175;
north[217] = 193;
north[216] = 210;
north[215] = 227;
north[214] = 250;
north[213] = 263;
north[212] = 282;
north[211] = 303;
north[210] = 317;
north[209] = 332;
north[208] = 352;
north[207] = 377;
north[206] = 328;
north[205] = 306;
north[204] = 292;
north[203] = 270;
north[202] = 254;
north[201] = 188;
north[200] = 195;
north[199] = 274;
north[198] = 300;
north[197] = 307;
north[196] = 377;
north[195] = 364;
north[194] = 350;
north[193] = 326;
north[192] = 290;
north[191] = 283;
north[190] = 266;
north[189] = 249;
north[188] = 233;
north[187] = 213;
north[186] = 196;
north[185] = 182;
north[184] = 165;
north[183] = 150;
north[182] = 132;
north[181] = 118;
north[180] = 100;
north[179] = 85;
north[178] = 65;
north[177] = 52;
north[176] = 34;
north[175] = 27;
north[174] = 29;
north[173] = 44;
north[172] = 55;
north[171] = 68;
north[170] = 88;
north[169] = 103;
north[168] = 119;
north[167] = 134;
north[166] = 145;
north[165] = 122;
north[164] = 107;
north[163] = 91;
north[162] = 76;
north[161] = 64;
north[160] = 54;
north[159] = 45;
north[158] = 52;
north[157] = 62;
north[156] = 70;
north[155] = 82;
north[154] = 97;
north[153] = 115;
north[152] = 131;
north[151] = 145;
north[150] = 165;
north[149] = 183;
north[148] = 199;
north[147] = 215;
north[146] = 230;
north[145] = 250;
north[144] = 266;
north[143] = 286;
north[142] = 305;
north[141] = 324;
north[140] = 346;
north[139] = 362;
north[138] = 380;
north[137] = 394;
north[136] = 390;
north[135] = 376;
north[134] = 365;
north[133] = 344;
north[132] = 324;
north[131] = 304;
north[130] = 284;
north[129] = 264;
north[128] = 244;
north[127] = 224;
north[126] = 204;
north[125] = 192;
north[124] = 174;
north[123] = 158;
north[122] = 145;
north[121] = 129;
north[120] = 116;
north[119] = 106;
north[118] = 92;
north[117] = 80;
north[116] = 70;
north[115] = 165;

north[114] = 184;
north[113] = 203;
north[112] = 218;
north[111] = 233;
north[110] = 250;
north[109] = 268;
north[108] = 288;
north[107] = 302;
north[106] = 318;
north[105] = 308;
north[104] = 284;
north[103] = 268;
north[102] = 252;
north[101] = 237;
north[100] = 221;
north[99] = 207;
north[98] = 194;
north[97] = 180;
north[96] = 166;
north[95] = 156;
north[94] = 146;
north[93] = 130;
north[92] = 179;
north[91] = 190;
north[90] = 205;
north[89] = 223;
north[88] = 240;
north[87] = 254;
north[86] = 268;
north[85] = 282;
north[84] = 293;
north[83] = 304;
north[82] = 316;
north[81] = 330;
north[80] = 344;
north[79] = 375;
north[78] = 362;
north[77] = 344;
north[76] = 226;
north[75] = 222;
north[74] = 208;
north[73] = 206;
north[72] = 198;
north[71] = 187;
north[70] = 187;
north[69] = 183;
north[68] = 179;
north[67] = 179;
north[66] = 180;
north[65] = 183;
north[64] = 219;
north[63] = 216;
north[62] = 214;
north[61] = 208;
north[60] = 211;
north[59] = 217;
north[58] = 223;
north[57] = 222;
north[56] = 226;
north[55] = 231;
north[54] = 236;
north[53] = 245;
north[52] = 260;
north[51] = 264;
north[50] = 282;


north[49] = 290;
north[48] = 300;
north[47] = 318;
north[46] = 304;
north[45] = 293;
north[44] = 289;
north[43] = 284;
north[42] = 280;
north[41] = 279;
north[40] = 279;
north[39] = 279;
north[38] = 280;
north[37] = 280;
north[36] = 282;
north[35] = 285;
north[34] = 255;
north[33] = 253;
north[32] = 254;
north[31] = 253;
north[30] = 251;
north[29] = 249;
north[28] = 247;
north[27] = 254;
north[26] = 264;
north[25] = 267;
north[24] = 272;
north[23] = 279;
north[22] = 280;
north[21] = 278;
north[20] = 283;
north[19] = 310;
north[18] = 315;
north[17] = 318;
north[16] = 320;
north[15] = 326;
north[14] = 327;
north[13] = 331;
north[12] = 331;
north[11] = 340;
north[10] = 270;
north[9] = 276;
north[8] = 265;
north[7] = 246;
north[6] = 230;
north[5] = 218;
north[4] = 168;
north[3] = 225;
north[2] = 239;
north[1] = 256;
north[0] = 270;

}


void setup() 
{
  FastLED.addLeds<WS2812B, DATA_PIN, LED_COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.setDither(1);
  FastLED.clear();

  setup_dist();
  
 setup_counter();

 last_time_of_wave = millis();
 
 Serial.begin (115200);
}


void show_led() {
  leds[CounterPos] = CRGB(0, brigtness_high, 0); 

    // show berlin
    if(berlin_offset == CounterPos) {
      leds[berlin_offset] = CRGB(brigtness_high, 0, 0);
    } else {
      leds[berlin_offset] = CRGB(0, 0, brigtness_normal);
    }
}


const int width = 30;
int wave_dist = 0;

void show_dist() {

  for(int i = 0; i < NUM_LEDS; ++i) {
    if(dist[i] >= wave_dist && dist[i] <= wave_dist + width) {
      leds[i] = CRGB(0, brigtness_normal, 0); 
    }
  }

  // allways show berlin
  leds[berlin_offset] = CRGB(brigtness_normal, 0, 0);
}

void show_wave() {

  for(int i = 0; i < NUM_LEDS; ++i) {
    if(dist[i] < wave_dist - width) {
      leds[i] = CRGB(0, 0, 0);
    } else if(dist[i] >= wave_dist) {
      leds[i] = CRGB(brigtness_normal/2, brigtness_normal, brigtness_normal); 
    } else {
      // interpolate
      float t = (((float)dist[i]) - (wave_dist - width)) / (float)(width);
      int c = constrain((int)(t*brigtness_high + 0.5f),0,255); 
      leds[i] = CRGB(c, c, c);
    }
  }

  // allways show berlin
  if(wave_dist - width > 0) {
    leds[berlin_offset] = CRGB(brigtness_weak, brigtness_weak, brigtness_weak);
  }
}

float animated_wave_dist = 0.0f;
const float animated_wave_step = 0.8f; // 1.0f

void animate_wave() 
{
  if(!do_animate_wave) {
    animated_wave_dist = 0.0f;
  } 
  else if(animated_wave_dist < 550) {
    animated_wave_dist += animated_wave_step;
  }
  
  wave_dist = (int) (animated_wave_dist+0.5f);
  //show_dist();
  show_wave();
}


void loop()
{ 
  update_counter();
  
  if(CounterPos != last_counter) {
    Serial.println(CounterPos);

    FastLED.clear();
    show_led();

    //wave_dist = CounterPos*width;
    //show_dist();

    //wave_dist = CounterPos;
    //show_wave();
    
    FastLED.show();

    // adjust brigtness
    //brigtness_high = constrain(255-CounterPos, 0, 255);
    //brigtness_normal = brigtness_high/4;
    //brigtness_weak = brigtness_high/16;
  }

  // automatic play mode
  /*
  if((do_animate_wave && animated_wave_dist > 500) || (!do_animate_wave && millis() - last_time_of_wave > 5000)) {
    do_animate_wave = !do_animate_wave;
    if(!do_animate_wave) {
      last_time_of_wave = millis();
    }
  }

  // movie:
  FastLED.clear();
  animate_wave();
  FastLED.show();
  */
   
  last_counter = CounterPos;
} 
