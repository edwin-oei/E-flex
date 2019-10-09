#include <LCD.h>
#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>

unsigned long startTime = 0;
int dummyBatteryLevel_percent = 50;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);           // set up Serial library at 9600 bps. This is the standard value.
  Serial.flush();             // Clear whatever output was before it
  startTime = millis();
  Serial.print("Start time: "); Serial.println(startTime);
  Serial.print("Initial battery level: "); Serial.print(dummyBatteryLevel_percent); Serial.println("%");
}

void loop() {
  // put your main code here, to run repeatedly:
  displayBatteryLevel_charging();
}


void displayBatteryLevel_charging()
{
  float static currentBatteryCharge_joules = 0;
  int currentBatteryLevel_percent;
  float initialBatteryCharge_joules = 180;
  
  //Serial.print("Current time: "); Serial.println(millis());
  currentBatteryCharge_joules = initialBatteryCharge_joules + (millis()-startTime)/1000*0.4;
  currentBatteryLevel_percent =  currentBatteryCharge_joules/360*100;
  //Serial.print("Battery level: "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
  
  if (currentBatteryLevel_percent == dummyBatteryLevel_percent){
    //Serial.print(millis());
    //Serial.print("Battery level: "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
  }
  
  else if (currentBatteryLevel_percent != dummyBatteryLevel_percent && currentBatteryLevel_percent - dummyBatteryLevel_percent == 2){
    Serial.print("Battery level: "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
    dummyBatteryLevel_percent = currentBatteryLevel_percent;  
  }
  
}
