#include <FastLED.h>
#define ledPIN     11
#define NUM_LEDS    18 // Numbering from 0 - 17
CRGB leds[NUM_LEDS];  // Set up the block of memory that will be used for storing and manipulating the led data (array)
uint8_t ledBrightness = 35;  // Brightness level ranges from 0 to 255


byte  stopEverythingPin = 8;
byte  RE_checkPin = A3;

void setup(){
  Serial.begin(19200);    
  FastLED.addLeds<WS2812, ledPIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(ledBrightness);
  FastLED.clear();  // Clears the led values from previous run
  pinMode(stopEverythingPin, INPUT);
  pinMode(RE_checkPin, INPUT);
}


void RUN_REenough() {
  leds[1] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[1] = CRGB::Black;
  leds[2] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[2] = CRGB::Black;  
  leds[4] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[4] = CRGB::Black;
  leds[5] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[5] = CRGB::Black;      
  leds[7] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[7] = CRGB::Black;  
  leds[8] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[8] = CRGB::Black;     
  // no energy flow from battery to system?
  leds[17] = CRGB(0,100,0);
  FastLED.show();
  delay(80);
  leds[17] = CRGB::Black;       
  leds[16] = CRGB(0,100,0);
  FastLED.show();
  delay(80);
  leds[16] = CRGB::Black;      
  leds[15] = CRGB(0,100,0);
  FastLED.show();
  delay(160);
  leds[15] = CRGB::Black; 
  FastLED.show();                                   
 }
 
void RUN_RENOTenough(){
  leds[13] = CRGB(188,143,143);
  FastLED.show();
  delay(80);
  leds[13] = CRGB::Black;  
  leds[12] = CRGB(188,143,143);
  FastLED.show();
  delay(80);
  leds[12] = CRGB::Black;
  leds[11] = CRGB(188,143,143);
  FastLED.show();
  delay(80);
  leds[11] = CRGB::Black;  
  leds[1] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[1] = CRGB::Black;
  leds[2] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[2] = CRGB::Black;      
  leds[4] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[4] = CRGB::Black;  
  leds[5] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[5] = CRGB::Black;     
  leds[7] = CRGB::SeaGreen;
  FastLED.show();
  delay(80);
  leds[7] = CRGB::Black;          
  leds[8] = CRGB::SeaGreen;
  FastLED.show();
  delay(160);
  leds[8] = CRGB::Black;
  FastLED.show();                  
}


void loop(){
  if (digitalRead(stopEverythingPin) == LOW){
    
    // RE enough or not pin
    if (10 < analogRead(RE_checkPin) && analogRead(RE_checkPin) < 20){  // Program start no leds on
      Serial.print(F("Program start. LEDs off.\n Re_checkPin = "));
      Serial.print(analogRead(RE_checkPin));
      FastLED.clear(); 
      FastLED.show();
    }
    else if (analogRead(RE_checkPin) < 8){
      leds[0] = CRGB::Red;     // 0: RE ; RE not enough --> red
      leds[14] = CRGB(255,69,0);     // 14: Battery ; RE not enough, battery discharging --> orange
      FastLED.show();
      Serial.print(F("RE not enough.\n Re_checkPin = "));
      Serial.print(analogRead(RE_checkPin));
      RUN_RENOTenough();
    }
    else if (analogRead(RE_checkPin) > 800){  // Re enough
      Serial.print(analogRead(RE_checkPin));
      leds[0] = CRGB::Green;     // 0: RE ; RE enough --> green
      leds[14] = CRGB::Green;     // 14: Battery ; RE enough, battery charging --> green
      FastLED.show();
      Serial.print(F("RE enough.\n Re_checkPin = "));
      Serial.print(analogRead(RE_checkPin));
      RUN_REenough();
    }  
  } 
  
  else{  // Stop everything
    fill_solid(leds, NUM_LEDS, CRGB(145,44,238));
    FastLED.show();
  }
}
