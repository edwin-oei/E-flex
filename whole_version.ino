// 2019.10.03
// Power required to operate of the whole system = 100W

#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);    //Set the LCD I2C address. These are default values. 0x27 is the device address.

int total_system_power = 100 // Power required to operate of the whole system = 100W
float maxWindPower_watts = 60
float windPower_watts;                         // in Watts
int windPower_share;                     // in %
float maxSolarPower_watts = 120
float solarPower_watts;                        // in Watts
int solarPower_share;                    // in %
float renewables_watts;            // in Watts

int userDemand;                          // 0 for no demand, 1 for low demand, 2 for high demand

int upperWaterLevelThreshold = 400
int lowerWaterLevelThreshold = 50

const int initialBatteryLevel_percent = 50 ;                    // initial Li-Ion. Battery name:18650. Values in %. 50% means 50 Watt hours.
int runningBatteryLevel_percent;
int dummyBatteryLevel_percent = 50;
float initialBatteryCharge_joules;           // in Joules. Describes the current level of battery charge in Watt-hr. 1Wh = 3600J
float currentBatteryCharge_joules;

unsigned long intime;
unsigned long extime;
char junk = ' ';
const int chargerPin = 2;        //relay for battery chargerPin  
const int pumpPin = 3;          //relay for submersible pump connect to pin 3，Plug into NC
const int valvePin = 7 ;         //relay for solenoid valvePin connect to pin7
const byte  ENA = 6;         //L298N pins setting, for conveyor DC motor. First pin
const byte  INA1 = 5;       // Second pin for motor
const byte  INA2 = 4;       // Third pin for motor
const int waterLevelPin = A3;          //Pin A3 is for the water level sensor. Why integer? Bcos it is given by manufacturer. A3 will be converted to integer on the board.

int currentWaterLevel = analogRead(waterLevelPin);


void setup()                    // run once when the sketch starts
{
  Serial.begin(9600);           // set up Serial library at 9600 bps. This is the standard value.
  Serial.flush();             // Clear whatever output was before it

  // Configure Arduino pin to be either input or output
  pinMode(waterLevelPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  pinMode(valvePin, OUTPUT);
  pinMode(INA1, OUTPUT);
  pinMode(INA2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // initialization of LCD
  lcd.begin(16, 2);         // Ours is a standard 16x2 LCD.
  lcd.home();      // Place cursor on first row and first column
  lcd.print("E-Flex Demo");
  delay(1200);
  lcd.clear();    // Clears the display and lcd memory
  lcd.home();
  lcd.print("Let's start!");
  delay(1500);
  lcd.clear();

 
  // Serial input of water demand, wind power share in per cent , solar power share in per cent
  Serial.println("Enter your demand for water here and press ENTER\n
                  0 : No demand),   1 : Low demand,   2 : High demand");
  
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character. This is like input for python. This lingo has no direct input function.
  {
    // input of user demand
    userDemand = Serial.parseInt();        // new command in 1.0 forward   Why parseInt????????????????????
    Serial.print("userDemand = "); Serial.println(userDemand, DEC);
    while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters
    { junk = Serial.read() ; }      // clear the keyboard buffer
  }
  
  Serial.println("Enter share of wind power in % and press ENTER");
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character
  {
    windPower_share = Serial.parseInt();        // new command in 1.0 forward
    Serial.print("Wind power share = "); Serial.print(windPower_share, DEC); Serial.println(" %.)  //Prints in DECimal format
    while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters. Not sure if this is necessary?????????????????
    { junk = Serial.read() ; }      // clear the keyboard buffer
  }

  Serial.println("Enter share of solar power in % and press ENTER");
  while (Serial.available() == 0) ;
  {
    solarPower_share = Serial.parseInt();
    Serial.print("Solar power share = "); Serial.print(solarPower_share, DEC); Serial.println(" %.)    
    while (Serial.available() > 0)
    { junk = Serial.read() ; }
  }

  windPower_watts = windPower_share/100*maxWindPower_watts;       // times a number to change % into watt, if wind=100%= 60W
  Serial.print("Wind power = "); Serial.print(windPower_watts, DEC); Serial.println(" watts");
  solarPower_watts = solarPower_share/100*maxSolarPower_watts;    // times a number to change % into watt, if solar=100%= 120W
  Serial.print("Solar power = "); Serial.print(solarPower_watts, DEC); Serial.println(" watts");
  renewables_watts = windPower_watts + solarPower_watts;
  Serial.print("Total renewables power = "); Serial.print(renewables_watts, DEC); Serial.println(" watts");

  lcd.clear(); lcd.home();
  lcd.print("Wind = "); lcd.print(windPower_watts); lcd.println(" watts");
  lcd.print("Solar = "); lcd.print(solarPower_watts); lcd.print(" watts");
  delay(1500);    
  lcd.clear(); lcd.home();
  lcd.print("Renewables = "); lcd.print(r);

  int initialWaterLevel = analogRead(waterLevelPin);
  Serial.print("Initial water level = "); Serial.println(initialWaterLevel);
  initialBatteryCharge_joules = initialBatteryLevel_percent*3600 ;  // current battery capacity here is 180 000 J if battery level = 50%
  Serial.print("Initial battery charge = "); Serial.print(batteryCharge_joules); Serial.println(" Joules");

  startTime = millis();
}


void loop()
{   
  if (renewables_watts >= total_system_power){  //Battery charging   
    switch (userDemand){
      case 2: // High demand
          while (currentWaterLevel >= upperWaterLevelThreshold){    // High water level
            if (currentBatteryLevel_percent <= 100){
              displayBatteryLevel_charging(startTime);
            }
            digitalWrite(pumpPin, LOW);    //  Turn off pump 
            Serial.println("Pump off, conveyor full speed");
            runMotorFast();  delay(2000);
            brakeMotor();
            digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
            digitalWrite(valvePin, LOW);
          }
          while (upperWaterLevelThreshold > currentWaterLevel){   // Medium or low water level
            if (currentBatteryLevel_percent <= 100){
              displayBatteryLevel_charging(startTime);
            }
            digitalWrite(pumpPin, HIGH);    //  Turn on pump 
            Serial.println("Pump on, conveyor full speed");
            runMotorFast();  delay(2000);
            brakeMotor();
            digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
            digitalWrite(valvePin, LOW);
          }

      case 1: // Low demand
          while (currentWaterLevel >= upperWaterLevelThreshold){                              // High water level
            if (currentBatteryLevel_percent <= 100){
              displayBatteryLevel_charging(startTime);
            }
            digitalWrite(pumpPin, LOW);    //  Turn off pump 
            Serial.println("Pump off, conveyor slow");
            runMotorSlow(); delay(4000);
            brakeMotor();
            digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
            digitalWrite(valvePin, LOW);
          }
          while (upperWaterLevelThreshold > currentWaterLevel){    // Medium or low water level
            if (currentBatteryLevel_percent <= 100){
              displayBatteryLevel_charging(startTime);
            }
            digitalWrite(pumpPin, HIGH);    //  Turn on pump
            Serial.println("Pump on, conveyor slow");
            runMotorSlow(); delay(4000);
            brakeMotor();
            digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
            digitalWrite(valvePin, LOW);            
          }

      case 0: // No demand
          while (currentWaterLevel >= upperWaterLevelThreshold){                              // High water level
            if (currentBatteryLevel_percent <= 100){
              displayBatteryLevel_charging(startTime);
            }
            digitalWrite(pumpPin, LOW);    //  Turn off pump 
            stopMotor();
            digitalWrite(valvePin, LOW);
            Serial.println("Pump off, conveyor stop");
          }
          while (upperWaterLevelThreshold > currentWaterLevel){    // Medium or low water level
            if (currentBatteryLevel_percent <= 100){
              displayBatteryLevel_charging(startTime);
            }
            digitalWrite(pumpPin, HIGH);    //  Turn on pump
            stopMotor();
            digitalWrite(valvePin, LOW);
            Serial.println("Pump on, conveyor stop");            
          }           
    }}
  else { // Renewables not enough 
    switch (userDemand){
      case 2:  // High demand
          while (currentWaterLevel >= lowerWaterLevelThreshold){
            if (displayBatteryLevel_discharging(startTime) >= 0){
              digitalWrite(pumpPin, LOW);    //  Turn off pump 
              Serial.println("Pump off, conveyor full speed");
              runMotorFast();  delay(2000);
              brakeMotor();
              digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
              digitalWrite(valvePin, LOW);
            }
            else {
              stopEverything();
            }
          }
          while (currentWaterLevel < lowerWaterLevelThreshold){
            if (displayBatteryLevel_discharging(startTime) >= 0){
              digitalWrite(pumpPin, HIGH);    //  Turn on pump 
              Serial.println("Pump on, conveyor full speed");
              runMotorFast();  delay(2000);
              brakeMotor();
              digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
              digitalWrite(valvePin, LOW);
            }
            else {
              stopEverything();
            }
          }
      case 1: // Low demand
          while (currentWaterLevel >= lowerWaterLevelThreshold){                // High or medium water level
            if (displayBatteryLevel_discharging(startTime) >= 0){
              digitalWrite(pumpPin, LOW);    //  Turn off pump 
              Serial.println("Pump off, conveyor slow");
              runMotorSlow(); delay(4000);
              brakeMotor();
              digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
              digitalWrite(valvePin, LOW);
            }
            else {
              stopEverything();
            }
          }
          while (currentWaterLevel < lowerWaterLevelThreshold){                 // Low water level
            if (displayBatteryLevel_discharging(startTime) >= 0){
              digitalWrite(pumpPin, HIGH);    //  Turn on pump
              Serial.println("Pump on, conveyor slow");
              runMotorSlow(); delay(4000);
              brakeMotor();
              digitalWrite(valvePin, HIGH);  delay(1000);  // Fill the bottle.
              digitalWrite(valvePin, LOW);
            }
            else {
              stopEverything();
            }
          }
      case 0: // No demand
          while (currentWaterLevel >= lowerWaterLevelThreshold){                // High or medium water level
            if (displayBatteryLevel_discharging(startTime) >= 0){
              digitalWrite(pumpPin, LOW);    //  Turn off pump 
              stopMotor();
              digitalWrite(valvePin, LOW);
              Serial.println("Pump off, conveyor stop");
            }
            else {
              stopEverything();
            }
          }
          while (currentWaterLevel < lowerWaterLevelThreshold){                 // Low water level
            if (displayBatteryLevel_discharging(startTime) >= 0){
              digitalWrite(pumpPin, HIGH);    //  Turn on pump
              stopMotor();
              digitalWrite(valvePin, LOW);
              Serial.println("Pump on, conveyor stop");
            }
            else {
              stopEverything();
            }
          }
    }
        
  }
}




//====================================
//start DC motor of conveyor with high speed-clockwise
void runMotorFast()
{
 analogWrite(ENA,255); //255 is the max output of analog write according to Arduino specs
 digitalWrite(INA1,HIGH);  //INA 1 high and INA2 low means clockwise
 digitalWrite(INA2,LOW);  
}
//start DC motor of conveyor with low speed-clockwise
void runMotorSlow()
{
 analogWrite(ENA,128);  
 digitalWrite(INA1,HIGH);
 digitalWrite(INA2,LOW);  
}
//brake DC motor of conveyor
void brakeMotor()
{
 analogWrite(ENA,255);  
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

int displayBatteryLevel_discharging(time startTime)
{
  int currentBatteryLevel_percent;
  currentBatteryCharge_joules = initialBatteryCharge_joules - (total_system_power-renewables_watts)*((millis()-startTime)/1000);
  currentBatteryLevel_percent =  currentBatteryCharge_joules/360000*100;

  if (dummyBatteryLevel_percent == initialBatteryLevel_percent){
    lcd.clear();
    lcd.home();
    lcd.print("Battery level: "); lcd.print(initialBatteryLevel_percent); lcd.print("%");
  }  
  else if (dummyBatteryLevel_percent != currentBatteryLevel_percent && (dummyBatteryLevel_percent - currentBatteryLevel_percent) % 2 == 0){
    if (currentBatteryLevel_percent > 0) {  
      lcd.clear();    // Clears the display and lcd memory
      lcd.home();
      lcd.print("Battery level: "); lcd.print(currentBatteryLevel_percent); lcd.print("%");
      dummyBatteryLevel_percent = currentBatteryLevel_percent;
    }
    else {
      lcd.clear();    // Clears the display and lcd memory
      lcd.home();
      lcd.print("Battery empty.");
    }
  }
  return currentBatteryLevel_percent;
}


int displayBatteryLevel_charging(time startTime)
{
  int currentBatteryLevel_percent;
  currentBatteryCharge_joules = initialBatteryCharge_joules + (renewables_watts-total_system_power)*((millis()-startTime)/1000);
  currentBatteryLevel_percent =  currentBatteryCharge_joules/360000*100;

  if (dummyBatteryLevel_percent == initialBatteryLevel_percent){
    lcd.clear();
    lcd.home();
    lcd.print("Battery level: "); lcd.print(initialBatteryLevel_percent); lcd.print("%");
  }  
  else if (dummyBatteryLevel_percent != currentBatteryLevel_percent && (currentBatteryLevel_percent - dummyBatteryLevel_percent) % 2 == 0){
    if (currentBatteryLevel_percent < 100) {
      lcd.clear();    // Clears the display and lcd memory
      lcd.home();
      lcd.print("Battery level: "); lcd.print(currentBatteryLevel_percent); lcd.print("%");
      dummyBatteryLevel_percent = currentBatteryLevel_percent;
    }
    else {
      lcd.clear();
      lcd.home();
      lcd.print("Battery full.");
    }
  }
  
  return currentBatteryLevel_percent
}
  

//====================================
  
