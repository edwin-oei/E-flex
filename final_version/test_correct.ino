#include <FastLED.h>
#define ledPIN     11
#define NUM_LEDS    18 // Numbering from 0 - 17
CRGB leds[NUM_LEDS];  // Set up the block of memory that will be used for storing and manipulating the led data (array)
uint8_t ledBrightness = 56;  // Brightness level ranges from 0 to 255


byte  stopEverythingPin = 8;
byte  RE_enoughPin = 2;

void setup(){
  FastLED.addLeds<WS2812, ledPIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(ledBrightness);
  FastLED.clear();  // Clears the led values from previous run

  pinMode(stopEverythingPin, INPUT);
  pinMode(RE_enoughPin, INPUT);
}


void RUN_REenough() {
  leds[1] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[1] = CRGB::Black;
  leds[2] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[2] = CRGB::Black;  
  leds[4] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[4] = CRGB::Black;
  leds[5] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[5] = CRGB::Black;      
  leds[7] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[7] = CRGB::Black;  
  leds[8] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[8] = CRGB::Black;     
  // no energy flow from battery to system?
  leds[17] = CRGB(0,100,0);
  FastLED.show();
  delay(40);
  leds[17] = CRGB::Black;       
  leds[16] = CRGB(0,100,0);
  FastLED.show();
  delay(40);
  leds[16] = CRGB::Black;      
  leds[15] = CRGB(0,100,0);
  FastLED.show();
  delay(40);
  leds[15] = CRGB::Black; 
  FastLED.show();                                   
 }
 
void RUN_RENOTenough(){
  leds[13] = CRGB(188,143,143);
  FastLED.show();
  delay(40);
  leds[13] = CRGB::Black;  
  leds[12] = CRGB(188,143,143);
  FastLED.show();
  delay(40);
  leds[12] = CRGB::Black;
  leds[11] = CRGB(188,143,143);
  FastLED.show();
  delay(40);
  leds[11] = CRGB::Black;  
  leds[1] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[1] = CRGB::Black;
  leds[2] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[2] = CRGB::Black;      
  leds[4] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[4] = CRGB::Black;  
  leds[5] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[5] = CRGB::Black;     
  leds[7] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[7] = CRGB::Black;          
  leds[8] = CRGB::SeaGreen;
  FastLED.show();
  delay(40);
  leds[8] = CRGB::Black;
  FastLED.show();                  
}


void loop(){
  if analogRead(stopEverythingPin) < 10{
    // RE enough or not pin
    if analogRead(RE_enoughPin) < 10{  // Program start no leds on
      leds[0] = CRGB::Black;     // 0: RE ; RE enough --> green
      leds[14] = CRGB::Black;     // 14: Battery ; RE enough, battery charging --> green
    }
    else if analogRead(RE_enoughPin) < 99{
      leds[0] = CRGB::Red;     // 0: RE ; RE not enough --> red
      leds[14] = CRGB(255,69,0);     // 14: Battery ; RE not enough, battery discharging --> orange
      FastLED.show();
      RUN_RENOTenough()
    }
    else{  // Re enough
      leds[0] = CRGB::Green;     // 0: RE ; RE enough --> green
      leds[14] = CRGB::Green;     // 14: Battery ; RE enough, battery charging --> green
      FastLED.show();
      RUN_REenough()
    }  
  } 
  
  else{  // Stop everything
    fill_solid(leds, NUM_LEDS, CRGB(145,44,238));
    FastLED.show();
  }
}
