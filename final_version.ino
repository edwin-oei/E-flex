// Power required to operate of the whole system = 0.1 kW
#include <LCD.h>
#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
  
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified module


float renewables_kiloWatts;            // in kiloWatts
byte userDemand;
char junk = ' ';
int initialBatteryLevel_percent = 50;
unsigned long startTime;
int dummyBatteryLevel_percent = initialBatteryLevel_percent;
int currentBatteryLevel_percent = initialBatteryLevel_percent;   

void setup(){
  char junk = ' ';
  const int waterLevelPin = A2;          //Pin A2 is for the water level sensor. Why integer? Bcos it is given by manufacturer. A3 will be converted to integer on the board.
  float maxWindPower_kiloWatts = 0.06;
  float windPower_kiloWatts;                         
  float windPower_share;                    
  float maxSolarPower_kiloWatts = 0.12;
  float solarPower_kiloWatts;                       
  float solarPower_share;     
              
  Serial.begin(19200);           // set up Serial library at 9600 bps. This is the standard value.
  Serial.flush();             // Clear whatever output was before it
  startTime = millis();
  Serial.print("Initial battery level: "); Serial.print(initialBatteryLevel_percent); Serial.println("%");
  
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
    
  lcd.print("Currently");
  lcd.setCursor(0,1);
  lcd.print(initialBatteryLevel_percent);
  lcd.setCursor(3,1);
  lcd.print("%");
    
  // User input required here ***************************************
  // Serial input of water demand, wind power share in per cent , solar power share in per cent
  Serial.println("\nEnter your demand for water here and press ENTER\n0 : No demand   1 : Low demand   2 : High demand");
    
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character. This is like input for python. This lingo has no direct input function.
    {
      // input of user demand
      userDemand = Serial.parseInt();        // new command in 1.0 forward
      
      if (userDemand == 2){
        Serial.print("\nUser demand = high\n");  
      }
      else if (userDemand == 1){
        Serial.print("\nUser demand = low\n");
      }
      else if (userDemand == 0){
        Serial.print("\nUser demand = none\n");
      }
  
      while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters. Not sure if this is necessary?????????????????
      { junk = Serial.read() ; }      // clear the keyboard buffer
    }
    

    Serial.println("Enter share of wind power in % and press ENTER");
    while (Serial.available() == 0) ;  // Wait here until input buffer has a character
    {
      windPower_share = Serial.parseInt();        // new command in 1.0 forward
      Serial.print("Wind power share = "); Serial.print(windPower_share); Serial.println("%");  //Prints in DECimal format
      while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters. Not sure if this is necessary?????????????????
      { junk = Serial.read() ; }      // clear the keyboard buffer
    }
  
    Serial.println("Enter share of solar power in % and press ENTER");
    while (Serial.available() == 0) ;
    {
      solarPower_share = Serial.parseInt();
      Serial.print("Solar power share = "); Serial.print(solarPower_share); Serial.println("%\n");    
      while (Serial.available() > 0)
      { junk = Serial.read() ; }
    }

  
  windPower_kiloWatts = windPower_share/100*maxWindPower_kiloWatts;       // times a number to change % into watt, if wind=100% = 0.06 kW
  Serial.print("Wind power = "); Serial.print(windPower_kiloWatts); Serial.println(" kiloWatts");
  solarPower_kiloWatts = solarPower_share/100*maxSolarPower_kiloWatts;    // times a number to change % into watt, if solar=100% = 0.12 kW
  Serial.print("Solar power = "); Serial.print(solarPower_kiloWatts); Serial.println(" kiloWatts\n");
  renewables_kiloWatts = windPower_kiloWatts + solarPower_kiloWatts;
  Serial.print("Total renewables power = "); Serial.print(renewables_kiloWatts); Serial.println(" kiloWatts\n");
    
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
}

//******************************************************************************************************************************************************************************
  
//brake DC motor of conveyor
void brakeMotor(byte ENA, byte INA1, byte INA2){
  analogWrite(ENA,120);  
  digitalWrite(INA1,LOW);      //LOW OR HIGH
  digitalWrite(INA2,LOW);      //LOW OR HIGH
}
  
  
void stopEverything(byte ENA, int pumpPin, int valvePin){
  analogWrite(ENA,0);
  digitalWrite(pumpPin, LOW);
  digitalWrite(valvePin, LOW);
  lcd.clear();
}


void runConveyor(byte ENA, byte INA1, byte INA2, int motorRPM, int lightSensorPin, int valvePin){
  analogWrite(ENA, motorRPM); //255 is the max output of analog write according to Arduino specs
  digitalWrite(INA1,HIGH);  //INA 1 high and INA2 low means clockwise
  digitalWrite(INA2,LOW);
  
  delay(800);
  
  if (analogRead(lightSensorPin) < 100){
    brakeMotor(ENA, INA1, INA2);
    
    digitalWrite(valvePin, HIGH);  // Valve open
    delay(2000);                  // Duration of open valve
    digitalWrite(valvePin, LOW);  // Valve close
    delay(500);                   // Buffer time before the ceonveyor moves again
  }
}


int displayBatteryLevel_charging(){
  float currentBatteryCharge_kiloJoules = 0;
  float initialBatteryCharge_kiloJoules = initialBatteryLevel_percent*360/100;
    
  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules + (millis()-startTime)/1000*0.4;
  currentBatteryLevel_percent =  currentBatteryCharge_kiloJoules/360*100;
    
  if (currentBatteryLevel_percent != dummyBatteryLevel_percent && currentBatteryLevel_percent - dummyBatteryLevel_percent == 2){
    lcd.clear();
    lcd.home();
    lcd.print("Charging");
    lcd.setCursor(0,1);
    lcd.print(currentBatteryLevel_percent);
    lcd.setCursor(3,1);
    lcd.print("%");
    dummyBatteryLevel_percent = currentBatteryLevel_percent;  
  }
  else if (currentBatteryLevel_percent >= 100) {
    lcd.clear();
    lcd.home();
    lcd.print("Fully charged");
  }

}


int displayBatteryLevel_discharging(){
  float currentBatteryCharge_kiloJoules = 0;
  float initialBatteryCharge_kiloJoules = initialBatteryLevel_percent*360/100;
  
  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules - (millis()-startTime)/1000*0.4;
  currentBatteryLevel_percent =  currentBatteryCharge_kiloJoules*100/360;
  
  if (currentBatteryLevel_percent != dummyBatteryLevel_percent &&  dummyBatteryLevel_percent - currentBatteryLevel_percent == 2){
    lcd.clear();
    lcd.home();
    lcd.print("Discharging");
    lcd.setCursor(0,1);
    lcd.print(currentBatteryLevel_percent);
    lcd.setCursor(3,1);
    lcd.print("%");
    dummyBatteryLevel_percent = currentBatteryLevel_percent;  
  }
  else if (currentBatteryLevel_percent <= 0) {
    lcd.clear();
    lcd.home();
    lcd.print("Battery empty."); delay(4000);
    lcd.setBacklightPin(5,POSITIVE);  // Turn off LCD
  }
  return currentBatteryLevel_percent;
}

//******************************************************************************************************************************************************************************


void loop(){
    const int waterLevelPin = A2;
    int lightSensorPin = A0;   
    float total_system_power = 0.1;
    int upperWaterLevelThreshold = 350;
    int lowerWaterLevelThreshold = 50;
    const int pumpPin = 3;          //relay for submersible pump connect to pin 3，Plug into NC
    const int valvePin = 7;         //relay for solenoid valvePin connect to pin7
    const byte  ENA = 6;         //L298N pins setting, for conveyor DC motor. First pin
    const byte  INA1 = 5;       // Second pin for motor
    const byte  INA2 = 4;       // Third pin for motor
    int slowMotorRPM = 180;
    int fastMotorRPM = 255;
    int i = 0;

    pinMode(waterLevelPin, INPUT);
    pinMode(pumpPin, OUTPUT);
    pinMode(valvePin, OUTPUT);
    pinMode(INA1, OUTPUT);
    pinMode(INA2, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(lightSensorPin, INPUT);

    Serial.print("Light value = "); Serial.println(analogRead(lightSensorPin));

    startTime = millis();
    
    if (renewables_kiloWatts >= total_system_power){  //Battery charging
      lcd.print("Charging");
      lcd.setCursor(0,1);
      lcd.print(initialBatteryLevel_percent);
      lcd.setCursor(3,1);
      lcd.print("%");   
      Serial.println("Renewables sufficient\n");
      switch (userDemand){
        case 0: // No demand
            
            while (i == 0){
              //runConveyor(ENA, INA1, INA2, 0, lightSensorPin, valvePin);  //At no demand, conveyor always stops, hence the 0 as input argument or the commented out line
              if (analogRead(waterLevelPin) >= upperWaterLevelThreshold){
                Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                digitalWrite(pumpPin, LOW);  // Pump off
                displayBatteryLevel_charging();
                Serial.print("No demand, high water level, charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");              
              }
              else {
                Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                digitalWrite(pumpPin, HIGH);  // Pump on
                displayBatteryLevel_charging();
                Serial.print("No demand, medium/low water level, charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
              }
            }
            
        case 1: // Low demand
            while (i == 0){
              runConveyor(ENA, INA1, INA2, slowMotorRPM, lightSensorPin, valvePin);  //At low demand, conveyor always runs slowly
              if (analogRead(waterLevelPin) >= upperWaterLevelThreshold){
                Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                digitalWrite(pumpPin, LOW);  // Pump off
                displayBatteryLevel_charging();
                Serial.print("Low demand, high water level, charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
              }
              else {
                Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                digitalWrite(pumpPin, HIGH);  // Pump on
                displayBatteryLevel_charging();
                Serial.print("Low demand, medium/low water level, charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
              }
              
            }
        
        case 2: // High demand
            while (i == 0){
              runConveyor(ENA, INA1, INA2, fastMotorRPM, lightSensorPin, valvePin);  //At high demand, conveyor always runs fast
              if (analogRead(waterLevelPin) >= lowerWaterLevelThreshold){
                Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                digitalWrite(pumpPin, LOW);  //Pump off
                displayBatteryLevel_charging();
                Serial.print("Pump off, High demand, medium/high water level, discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%\n");
                
              }
              else{
                Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                digitalWrite(pumpPin, HIGH);    //Pump on
                displayBatteryLevel_charging();
                Serial.print("Pump On, High demand, medium/low level, charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%\n");
              }
            }
      }
    }
    else if (renewables_kiloWatts < total_system_power){ // Renewables not enough 
      lcd.print("Discharging");
      lcd.setCursor(0,1);
      lcd.print(initialBatteryLevel_percent);
      lcd.setCursor(3,1);
      lcd.print("%");  
      Serial.println("Renewables insufficient\n");
      switch (userDemand){
         case 0: // No demand
            while (i == 0){
              //runConveyor(ENA, INA1, INA2, 0, lightSensorPin, valvePin);  //At high demand, conveyor always stops. Hence the 0 input argument or that this is commented out.
              if (analogRead(waterLevelPin) >= lowerWaterLevelThreshold){                // High or medium water level
                if (displayBatteryLevel_discharging() >= 0){
                  Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                  digitalWrite(pumpPin, LOW);    //Pump off
                  Serial.print("No demand, medium/high water level, discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything(ENA, pumpPin, valvePin);
                }
              }
              else{
                if (displayBatteryLevel_discharging() >= 0){
                  Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                  digitalWrite(pumpPin, HIGH);    //Pump on
                  Serial.print("No demand, low water level, discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything(ENA, pumpPin, valvePin);
                }    
              }
            }
            
        case 1: // Low demand
            while (i == 0){
              runConveyor(ENA, INA1, INA2, slowMotorRPM, lightSensorPin, valvePin);  //At low demand, conveyor always runs slowly
              if (analogRead(waterLevelPin) >= lowerWaterLevelThreshold){                // High or medium water level
                if (displayBatteryLevel_discharging() >= 0){
                  Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                  digitalWrite(pumpPin, LOW);    //Pump off
                  Serial.print("Low demand, medium/high water level, discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything(ENA, pumpPin, valvePin);
                }
              }
              else{
                if (displayBatteryLevel_discharging() >= 0){
                  Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                  digitalWrite(pumpPin, HIGH);    //Pump on
                  Serial.print("Low demand, low water level, discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything(ENA, pumpPin, valvePin);
                } 
              }
            }
            
        case 2:  // High demand
            while (i == 0){
              runConveyor(ENA, INA1, INA2, fastMotorRPM, lightSensorPin, valvePin);  //At high demand, conveyor always runs fast
              if (analogRead(waterLevelPin) >= lowerWaterLevelThreshold){
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                  digitalWrite(pumpPin, LOW);    //Pump off
                  Serial.print("Pump off, medium/high water level, discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything(ENA, pumpPin, valvePin);
                }  
              }
              else{
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(analogRead(waterLevelPin));
                  digitalWrite(pumpPin, HIGH);    //Pump on
                  Serial.print("Pump on, High demand, low water level, discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything(ENA, pumpPin, valvePin);
                } 
              }
            }
             
   
      }
          
    }
}
  
