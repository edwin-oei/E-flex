#include <LCD.h>
#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified module

unsigned long startTime = 0;
unsigned long dummyTime = 0;
int dummyBatteryLevel_percent = 50;
int batteryState = 1; // 1 for charging, 0 for discharging

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);           // set up Serial library at 9600 bps. This is the standard value.
  Serial.flush();             // Clear whatever output was before it
  startTime = millis();
  Serial.print("Start time: "); Serial.println(startTime);
  Serial.print("Initial battery level: "); Serial.print(dummyBatteryLevel_percent); Serial.println("%");

  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(LOW); // NOTE: You can turn the backlight off by setting it to LOW instead of HIGH
  lcd.begin(16, 2);
  lcd.clear();
    
  lcd.print("E-Flex Demo");
  delay(2000);
  lcd.clear();    // Clears the display and lcd memory
  lcd.home();
  lcd.print("Let's start!");
  delay(1500);
  lcd.clear();
  lcd.print("Currently 50%");
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long timeNow = millis();
  
  if (timeNow - dummyTime == 300000){
    batteryState = batteryState*(-1);
    dummyTime = timeNow;
  }

  if (batteryState == 1) {
    displayBatteryLevel_charging();  
  }
  else {
    displayBatteryLevel_discharging();
  }
  
}


// Functions on what to display on the serial monitor

void displayBatteryLevel_charging()
{
  float static currentBatteryCharge_kiloJoules = 0;
  int currentBatteryLevel_percent;
  float initialBatteryCharge_kiloJoules = 180;
  
  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules + (millis()-startTime)/1000*0.6;
  currentBatteryLevel_percent =  currentBatteryCharge_kiloJoules/360*100;
  
  if (currentBatteryLevel_percent != dummyBatteryLevel_percent && currentBatteryLevel_percent - dummyBatteryLevel_percent == 2){
    Serial.print("Charging"); Serial.print(currentBatteryLevel_percent); Serial.println("%");
    lcd.clear();
    lcd.home();
    lcd.print("Charging");
    lcd.setCursor(0,1);
    lcd.print(currentBatteryLevel_percent);
    lcd.setCursor(2,1);
    lcd.print("%");
    dummyBatteryLevel_percent = currentBatteryLevel_percent;  
  }
   if (currentBatteryLevel_percent >= 100) {
   lcd.clear();
   lcd.home();
   lcd.print("Fully charged");
  }
}

void displayBatteryLevel_discharging()
{
  float static currentBatteryCharge_kiloJoules = 0;
  int currentBatteryLevel_percent;
  float initialBatteryCharge_kiloJoules = 180;
  
  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules - (millis()-startTime)/1000*0.4;
  currentBatteryLevel_percent =  currentBatteryCharge_kiloJoules/360*100;
  
  if (currentBatteryLevel_percent != dummyBatteryLevel_percent && currentBatteryLevel_percent - dummyBatteryLevel_percent == 2){
    Serial.print("Discharging"); Serial.print(currentBatteryLevel_percent); Serial.println("%");
    lcd.clear();
    lcd.home();
    lcd.print("Charging");
    lcd.setCursor(0,1);
    lcd.print(currentBatteryLevel_percent);
    lcd.setCursor(2,1);
    lcd.print("%");
    dummyBatteryLevel_percent = currentBatteryLevel_percent;  
  }
  if (currentBatteryLevel_percent <= 0) {
    lcd.clear();
    lcd.home();
    lcd.print("Battery empty");
    lcd.setCursor(0,1);
    lcd.print("Shutting down");
  }
}
