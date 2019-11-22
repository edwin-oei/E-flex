#include <LCD.h>
#include "time.h"
#include "math.h"               
#include<Wire.h>
#include<LiquidCrystal_I2C.h>
  
LiquidCrystal_I2C  lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified module. The other values in the brackets are standard

// The variable type here is set as constant to make it easier for future programmers to understand the code.
int lowerWaterLevelThreshold = 3;    // Anything at or below this is considered low water level, ie 50 < medium water level < 500
int upperWaterLevelThreshold = 6;   // Anything at or above this is considered high water level
int slowMotorRPM = 255;    // Arduino behaves weirdly here. 255 is slower than 200
int fastMotorRPM = 200;
const int trigPin = 10;
const int echoPin = 9;
int lightSensorPin = A0;   
int pumpPin = 3;          //relay for submersible pump connect to pin 3，Plug into NC
int valvePin = 7;         //relay for solenoid valvePin connect to pin 7
byte  ENA = 6;         //L298N pins setting, for conveyor DC motor. First pin
byte  INA1 = 5;       // Second pin for motor
byte  INA2 = 4;       // Third pin for motor

// The values below can be adjusted as how the team sees fit
float systemPower_kiloWatts = 0.1;    // Total power needed to run the entire system
float maxSolarPower_kiloWatts = 0.12; // Max power that can be supplied by the solar panels
float maxWindPower_kiloWatts = 0.06;

int initialBatteryLevel_percent = 50;
float renewables_kiloWatts;            // Value here is the sum of solar and wind power. The maths will be done in the setup loop
byte userDemand;
unsigned long startTime;    // Placeholder for time in milliseconds since the code started running
int dummyBatteryLevel_percent = initialBatteryLevel_percent;    // We are displaying battery change every 2% on the LCD, this variable is needed to store what is being displayed on the LCD
int currentBatteryLevel_percent = initialBatteryLevel_percent;   // The real current battery level

void setup(){
  pinMode(trigPin, OUTPUT); // Sets the trigPinPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPinPin as an Input    
  pinMode(pumpPin, OUTPUT);
  pinMode(valvePin, OUTPUT);
  pinMode(INA1, OUTPUT);
  pinMode(INA2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(lightSensorPin, INPUT);
  
  float windPower_kiloWatts;                         
  float windPower_share;                    
  float solarPower_kiloWatts;                       
  float solarPower_share;     

  char junk = ' ';            // Needed to process user input properly
              
  Serial.begin(19200);           // Set up Serial library at 19200 bps. This is more than the usual 9600
  Serial.flush();             
  Serial.print("Initial battery level: "); Serial.print(initialBatteryLevel_percent); Serial.println("%");
  
  lcd.setBacklightPin(3,POSITIVE);  // Turn on LCD backlight
  lcd.setBacklight(LOW); // Just a setting. Keep it this way
  lcd.begin(16, 2); // Ours is a standard 16 columns 2 rows LCD display
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
    
  
  // Serial input of water demand, wind power share in per cent , solar power share in per cent
  Serial.println("\nEnter your demand for water here and press ENTER\n0 : No demand   1 : Low demand   2 : High demand");
    
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character. This is like input for python. This lingo has no direct input function.
    {
      // input of user demand
      userDemand = Serial.parseInt();        // new command in 1.0 forward
      
      if (userDemand == 0){
        Serial.print("\nUser demand = low\n");  
      }
      else if (userDemand == 1){
        Serial.print("\nUser demand = low\n");
      }
      else if (userDemand == 2){
        Serial.print("\nUser demand = high\n");
      }
  
      while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters. Not sure if this is necessary?????????????????
      { junk = Serial.read() ; }      // clear the keyboard buffer
    }
    

    Serial.println("Enter wind power in % and press ENTER");
    while (Serial.available() == 0) ;  // Wait here until input buffer has a character
    {
      windPower_share = Serial.parseInt();        // new command in 1.0 forward
      Serial.print("Wind power = "); Serial.print(windPower_share); Serial.println("%");  //Prints in DECimal format
      while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters. Not sure if this is necessary?????????????????
      { junk = Serial.read() ; }      // clear the keyboard buffer
    }
  
    Serial.println("Enter solar power in % and press ENTER");
    while (Serial.available() == 0) ;
    {
      solarPower_share = Serial.parseInt();
      Serial.print("Solar power = "); Serial.print(solarPower_share); Serial.println("%\n");    
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
   
  int waterLevel = computeWaterLevel();
  Serial.print("Initial water level = "); Serial.println(waterLevel); 
}

//******************************************************************************************************************************************************************************

 int computeWaterLevel(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(8);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);                  // Sets the trigPinPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, LOW);
  int waterLevel = 14 - pulseIn(echoPin, HIGH)*0.034/2; // unit:cm, speed out sound=340 m/s= 0.034 cm/ms. 30 is to be measured, should equal the tank height
  //Serial.print("distance:");
  //Serial.print(distance);
  //Serial.println("cm");
  delay(300);
  return waterLevel;
 }


//brake DC motor of conveyor
void brakeMotor(){
  analogWrite(ENA,200);        // 120 in place of 0 is chosen to keep some current running into the motor. May reduce response time when motor is activated again
  digitalWrite(INA1,LOW);      //LOW and LOW causes the motor to stop running 
  digitalWrite(INA2,LOW);      
}
  
  
void stopEverything(){
  analogWrite(ENA,0);
  digitalWrite(pumpPin, LOW);
  digitalWrite(valvePin, LOW);
  lcd.clear();
}

void runConveyor(int motorRPM){
  analogWrite(ENA, motorRPM);
  digitalWrite(INA1,HIGH);  //INA 1 high and INA2 low means clockwise
  digitalWrite(INA2,LOW);

  if (motorRPM == fastMotorRPM){
    delay(850);
  }
  else{
    delay(725);
  }
  
  if (analogRead(lightSensorPin) < 100){
    brakeMotor();
    
    digitalWrite(valvePin, HIGH);  // Valve open
    delay(500);                  // Duration of open valve
    digitalWrite(valvePin, LOW);  // Valve close
    delay(2000);                   // Buffer time before the ceonveyor moves again
  }
  
  }



int displayBatteryLevel_charging(){
  float currentBatteryCharge_kiloJoules = 0;
  float initialBatteryCharge_kiloJoules = initialBatteryLevel_percent*360/100;
    
  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules + (millis()-startTime)/1000*(renewables_kiloWatts-systemPower_kiloWatts);
  currentBatteryLevel_percent =  currentBatteryCharge_kiloJoules/360*100;
    
  if (currentBatteryLevel_percent != dummyBatteryLevel_percent && currentBatteryLevel_percent - dummyBatteryLevel_percent == 1){
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
  
  currentBatteryCharge_kiloJoules = initialBatteryCharge_kiloJoules - (millis()-startTime)/1000*(systemPower_kiloWatts-renewables_kiloWatts);
  currentBatteryLevel_percent =  currentBatteryCharge_kiloJoules*100/360;
  
  if (currentBatteryLevel_percent != dummyBatteryLevel_percent &&  dummyBatteryLevel_percent - currentBatteryLevel_percent == 1){
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
    int i = 0;

    startTime = millis();
    
    if (renewables_kiloWatts >= systemPower_kiloWatts){   //Enough renewables to power the whole system
      if (renewables_kiloWatts == systemPower_kiloWatts){
        lcd.print("Battery stable");
      }
      else if (renewables_kiloWatts > systemPower_kiloWatts){
        lcd.print("Charging");
        lcd.setCursor(0,1);
        lcd.print(initialBatteryLevel_percent);
        lcd.setCursor(3,1);
        lcd.print("%");
      }
         
      Serial.println("Renewables sufficient\n\n");
      switch (userDemand){
        case 0: // No demand
            
            while (i == 0){
              if (computeWaterLevel() > upperWaterLevelThreshold){
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, LOW);  // Pump off
                displayBatteryLevel_charging();
                Serial.print("High water level, no demand, conveyor stop. Charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");              
              }
              else {
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, HIGH);  // Pump on
                displayBatteryLevel_charging();
                Serial.print("Medium/low water level, pump on, conveyor stop. Charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
              }
            }
            
        case 1: // Low demand
            while (i == 0){
              if (computeWaterLevel() > upperWaterLevelThreshold){
                runConveyor(slowMotorRPM);  //At low demand, conveyor always runs slowly
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, LOW);  // Pump off
                displayBatteryLevel_charging();
                Serial.print("High water level, pump off, conyeor slow. Charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
              }
              else if (upperWaterLevelThreshold >= computeWaterLevel() && computeWaterLevel() >= lowerWaterLevelThreshold){
                runConveyor(slowMotorRPM);  //At low demand, conveyor always runs slowly
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, HIGH);  // Pump on
                displayBatteryLevel_charging();
                Serial.print("Medium water level, pump on, conveyor slow. Charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
              }
              else if (computeWaterLevel() < lowerWaterLevelThreshold){
                brakeMotor();
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, HIGH);  // Pump on
                displayBatteryLevel_charging();
                Serial.print("Low water level, pump on, conveyor stop. Charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
              }             
            }
        
        case 2: // High demand
            while (i == 0){
              if (computeWaterLevel() > upperWaterLevelThreshold){
                runConveyor(fastMotorRPM);
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, LOW);  //Pump off
                displayBatteryLevel_charging();
                Serial.print("High water level, pump off, conveyor fast. Charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%\n");
              }
              else if (upperWaterLevelThreshold >= computeWaterLevel() && computeWaterLevel() >= lowerWaterLevelThreshold){
                runConveyor(fastMotorRPM);
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, HIGH);  //Pump on
                displayBatteryLevel_charging();
                Serial.print("Medium water level, pump on, conveyor fast, charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%\n");
              }
              else if (computeWaterLevel() < lowerWaterLevelThreshold){
                brakeMotor();
                Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                digitalWrite(pumpPin, HIGH);    //Pump on
                displayBatteryLevel_charging();
                Serial.print("Low water level, pump on, conveyor stop. Charging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%\n");
              }
            }
      }
    }
    else if (renewables_kiloWatts < systemPower_kiloWatts){ // Renewables not enough 
      lcd.print("Discharging");
      lcd.setCursor(0,1);
      lcd.print(initialBatteryLevel_percent);
      lcd.setCursor(3,1);
      lcd.print("%");  
      Serial.println("Renewables insufficient\n\n");
      switch (userDemand){
         case 0: // No demand
            while (i == 0){
              if (computeWaterLevel() >= lowerWaterLevelThreshold){                // High or medium water level
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                  digitalWrite(pumpPin, LOW);    //Pump off
                  Serial.print("Medium/high water level, pump off, conveyor stop. Discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything();
                }
              }
              else{
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                  digitalWrite(pumpPin, HIGH);    //Pump on
                  Serial.print("Low water level, pump on, conveyor stop. Discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything();
                }    
              }
            }
            
        case 1: // Low demand
            while (i == 0){
              if (computeWaterLevel() >= lowerWaterLevelThreshold){                // High or medium water level
                runConveyor(slowMotorRPM);
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                  digitalWrite(pumpPin, LOW);    //Pump off
                  Serial.print("Medium/high water level, pump off, conveyor slow. Discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything();
                }
              }
              else{
                brakeMotor();
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                  digitalWrite(pumpPin, HIGH);    //Pump on
                  Serial.print("Low water level, pump on, conveyor stop. Discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything();
                } 
              }
            }
            
        case 2:  // High demand
            while (i == 0){
              if (computeWaterLevel() >= lowerWaterLevelThreshold){
                runConveyor(fastMotorRPM);  //At high demand, conveyor always runs fast
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                  digitalWrite(pumpPin, LOW);    //Pump off
                  Serial.print("Medium/high water level, pump off, conveyor fast. Discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything();
                }  
              }
              else{
                brakeMotor();
                if (displayBatteryLevel_discharging() > 0){
                  Serial.print("Water level = "); Serial.println(computeWaterLevel()); Serial.println("cm");
                  digitalWrite(pumpPin, HIGH);    //Pump on
                  Serial.print("Low water level, pump on, conveyor stop. Discharging from "); Serial.print(currentBatteryLevel_percent); Serial.println("%");
                }
                else {
                  stopEverything();
                } 
              }
            }
             
   
      }
          
    }
}
  
