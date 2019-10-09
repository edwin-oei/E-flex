// Power required to operate of the whole system = 100W
#include <LCD.h>
#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified module

int total_system_power = 0.1; // Power required to operate of the whole system = 0.1 kW
float maxWindPower_kiloWatts = 0.06;
float windPower_kiloWatts;                         // in kiloWatts
float windPower_share;                     // in %
float maxSolarPower_kiloWatts = 0.12;
float solarPower_kiloWatts;                        // in kiloWatts
float solarPower_share;                    // in %
float renewables_kiloWatts;            // in kiloWatts

unsigned long startTime = 0;
unsigned long dummyTime = 0;
int dummyBatteryLevel_percent = 50;
int batteryState = 1; // 1 for charging, 0 for discharging

int userDemand;                          // 0 for no demand, 1 for low demand, 2 for high demand

char junk = ' ';

int upperWaterLevelThreshold = 350;
int lowerWaterLevelThreshold = 50;

int slowMotorRPM = 120;
int fastMotorRPM = 255;

const int pumpPin = 3;          //relay for submersible pump connect to pin 3，Plug into NC
const int valvePin = 7;         //relay for solenoid valvePin connect to pin7
const byte  ENA = 6;         //L298N pins setting, for conveyor DC motor. First pin
const byte  INA1 = 5;       // Second pin for motor
const byte  INA2 = 4;       // Third pin for motor
const int waterLevelPin = A2;          //Pin A3 is for the water level sensor. Why integer? Bcos it is given by manufacturer. A3 will be converted to integer on the board.

int currentWaterLevel = analogRead(waterLevelPin); // MIGHT be a problem?????????????????

int newUserDemand;

// Setup. Code will only run once
void setup(){
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


  // User input required here ***************************************
  // Serial input of water demand, wind power share in per cent , solar power share in per cent
  Serial.println("Enter your demand for water here and press ENTER\n0 : No demand)   1 : Low demand   2 : High demand");
  
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character. This is like input for python. This lingo has no direct input function.
  {
    // input of user demand
    userDemand = Serial.parseInt();        // new command in 1.0 forward   Why parseInt????????????????????
    Serial.print("User demand = "); Serial.println(userDemand, DEC);
    newUserDemand = userDemand;
    while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters. Not sure if this is necessary?????????????????
    { junk = Serial.read() ; }      // clear the keyboard buffer
  }
  
  Serial.println("Enter share of wind power in % and press ENTER");
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character
  {
    windPower_share = Serial.parseInt();        // new command in 1.0 forward
    Serial.print("Wind power share = "); Serial.print(windPower_share, DEC); Serial.println("%");  //Prints in DECimal format
    while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters. Not sure if this is necessary?????????????????
    { junk = Serial.read() ; }      // clear the keyboard buffer
  }

  Serial.println("Enter share of solar power in % and press ENTER");
  while (Serial.available() == 0) ;
  {
    solarPower_share = Serial.parseInt();
    Serial.print("Solar power share = "); Serial.print(solarPower_share, DEC); Serial.println("%");    
    while (Serial.available() > 0)
    { junk = Serial.read() ; }
  }

  windPower_kiloWatts = windPower_share/100*maxWindPower_kiloWatts;       // times a number to change % into watt, if wind=100% = 0.06 kW
  Serial.print("Wind power = "); Serial.print(windPower_kiloWatts); Serial.println(" kiloWatts");
  solarPower_kiloWatts = solarPower_share/100*maxSolarPower_kiloWatts;    // times a number to change % into watt, if solar=100% = 0.12 kW
  Serial.print("Solar power = "); Serial.print(solarPower_kiloWatts); Serial.println(" kiloWatts");
  renewables_kiloWatts = windPower_kiloWatts + solarPower_kiloWatts;
  Serial.print("Total renewables power = "); Serial.print(renewables_kiloWatts); Serial.println(" kiloWatts");

  lcd.clear(); lcd.home();
  lcd.print("Wind power");
  lcd.setCursor(0,1);
  lcd.print(windPower_kiloWatts); lcd.println(" kiloWatts");
  delay(1500);
  lcd.clear(); lcd.home();
  lcd.print("Solar power");
  lcd.setCursor(0,1);
  lcd.print(solarPower_kiloWatts); lcd.print(" kiloWatts");
  delay(1500);    
  lcd.clear(); lcd.home();
  lcd.print("Renewables");
  lcd.setCursor(0,1);
  lcd.print(renewables_kiloWatts);
  delay(1500);
  lcd.clear();
  lcd.home();
  
  int waterLevel = analogRead(waterLevelPin);
  Serial.print("Initial water level = "); Serial.println(waterLevel);
  Serial.print("Initial battery level = "); Serial.print(dummyBatteryLevel_percent); Serial.println("%"); 
}

//***************************************************************************************************************************************************************
// Functions *********************************************************************************
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

int displayBatteryLevel_discharging()
{
  float static currentBatteryCharge_kiloJoules = 0;
  int currentBatteryLevel_percent;
  float initialBatteryCharge_kiloJoules = 180;
  
  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules - (millis()-startTime)/1000*0.6;
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

  return currentBatteryLevel_percent;
}

void runMotorFast()
{
 analogWrite(ENA,fastMotorRPM); //255 is the max output of analog write according to Arduino specs
 digitalWrite(INA1,HIGH);  //INA 1 high and INA2 low means clockwise
 digitalWrite(INA2,LOW);  
}
//start DC motor of conveyor with low speed-clockwise
void runMotorSlow()
{
 analogWrite(ENA,slowMotorRPM);  
 digitalWrite(INA1,HIGH);
 digitalWrite(INA2,LOW);  
}
//brake DC motor of conveyor
void brakeMotor()
{
 analogWrite(ENA,fastMotorRPM);  
 digitalWrite(INA1,LOW);      //LOW OR HIGH
 digitalWrite(INA2,LOW);      //LOW OR HIGH
}

//stop DC motor of conveyor
void stopMotor()
{
  analogWrite(ENA,0);
}

// No battery, stop everything
void stopEverything()
{
  analogWrite(ENA,0);
  digitalWrite(pumpPin, LOW);
  digitalWrite(valvePin, LOW);
  lcd.clear();
}


void pumpOnConveyorFast() {
  digitalWrite(pumpPin, HIGH);    //  Turn on pump 
  Serial.println("Pump on, conveyor full speed");
  runMotorFast();  delay(2000);
  brakeMotor();
  digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
  digitalWrite(valvePin, LOW);
}

void pumpOffConveyorFast() {
  digitalWrite(pumpPin, LOW);    //  Turn off pump 
  Serial.println("Pump off, conveyor full speed");
  runMotorFast();  delay(2000);
  brakeMotor();
  digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
  digitalWrite(valvePin, LOW); 
}

void pumpOnConveyerSlow() {
  digitalWrite(pumpPin, HIGH);    //  Turn on pump
  Serial.println("Pump on, conveyor slow");
  runMotorSlow(); delay(4000);
  brakeMotor();
  digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
  digitalWrite(valvePin, LOW);
}

void pumpOffConveyorSlow() {
  digitalWrite(pumpPin, LOW);    //  Turn off pump 
  Serial.println("Pump off, conveyor slow");
  runMotorSlow(); delay(4000);
  brakeMotor();
  digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
  digitalWrite(valvePin, LOW);
}

void pumpOnConveyerOff() {
  digitalWrite(pumpPin, HIGH);    //  Turn on pump
  stopMotor();
  digitalWrite(valvePin, LOW);
  Serial.println("Pump on, conveyor stop");
}

void pumpOffConveyerOff() {
  digitalWrite(pumpPin, LOW);    //  Turn off pump 
  stopMotor();
  digitalWrite(valvePin, LOW);
  Serial.println("Pump off, conveyor stop");
}

//******************************************************************************************************************************************************************

void loop()
{   
  int static newDemand = userDemand;
  int i = 0;
  Serial.print("new user demand = "); Serial.println(newUserDemand); delay(3000);
  if (renewables_kiloWatts >= total_system_power){  //Battery charging   
    switch (newDemand){
      case 0: // No demand
          
          while (i == 0){
            if (analogRead(waterLevelPin) >= upperWaterLevelThreshold){
              displayBatteryLevel_charging();
              Serial.println("No demand, high water level, charging.");
              Serial.println(analogRead(waterLevelPin));
              pumpOffConveyerOff();
            }
            else {
              displayBatteryLevel_charging();
              pumpOnConveyerOff();         
              Serial.println("No demand, medium low water level, charging.");
              Serial.println(analogRead(waterLevelPin));
            }
          }
          
      case 1: // Low demand
          while (i == 0){
            if (analogRead(waterLevelPin) >= upperWaterLevelThreshold){
              displayBatteryLevel_charging();
              pumpOffConveyorSlow();
              Serial.println("Low demand, high water level, charging.");
              Serial.println(analogRead(waterLevelPin));
              Serial.println("");
            }
            else {
              displayBatteryLevel_charging();
              pumpOnConveyerSlow();        
              Serial.println("Low demand, medium low water level, charging.");
              Serial.println(analogRead(waterLevelPin));
              Serial.println("");  
            }
            
          }
      
      case 2: // High demand
          while (i == 0){
            if (analogRead(waterLevelPin) >= upperWaterLevelThreshold){
              displayBatteryLevel_charging();
              pumpOffConveyorFast();
              Serial.println("High demand, high water level, charging.");
              Serial.println(analogRead(waterLevelPin));
            }
            else {
              displayBatteryLevel_charging();
              pumpOnConveyorFast();
              Serial.println("High demand, medium low water level, charging.");
              Serial.println(analogRead(waterLevelPin));
            }
          }
    }
  }
  else { // Renewables not enough 
    switch (userDemand){
      case 2:  // High demand
          while (i == 0){
            if (analogRead(waterLevelPin) >= lowerWaterLevelThreshold){
              if (displayBatteryLevel_discharging() > 0){
                pumpOffConveyorFast();
              }
              else {
                stopEverything();
              }  
              }
            else{
             if (displayBatteryLevel_discharging() > 0){
              pumpOnConveyorFast();
              }
              else {
                stopEverything();
              } 
            }
          }
          
      case 1: // Low demand
          while (i == 0){
            if (analogRead(waterLevelPin) >= lowerWaterLevelThreshold){                // High or medium water level
              if (displayBatteryLevel_discharging() >= 0){
                pumpOffConveyorSlow();
              }
              else {
                stopEverything();
              }
            }
            else{
              if (displayBatteryLevel_discharging() >= 0){
                pumpOnConveyerSlow();
              }
              else {
                stopEverything();
              } 
            }
          }
          
      case 0: // No demand
          while (i == 0){
            if (analogRead(waterLevelPin) >= lowerWaterLevelThreshold){                // High or medium water level
              if (displayBatteryLevel_discharging() >= 0){
                pumpOffConveyerOff();
              }
              else {
                stopEverything();
              }
            }
            else{
              if (displayBatteryLevel_discharging() >= 0){
                pumpOnConveyerOff();
              }
              else {
                stopEverything();
              }    
            }
          }
           
 
    }
        
  }
}



