#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>

const int valvePin = 7;

void setup(){
  Serial.begin(9600);
  Serial.flush();
}


void loop(){
  digitalWrite(valvePin, HIGH); delay(3000);
  digitalWrite(valvePin, LOW); delay(3000);
}
