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
float solarPower_watts;                        // in Watt
int solarPower_share;                    // in %
int batteryLevel_percent = 50 ;                    // initial Li-Ion. Battery name:18650. Values in %. 50% means 50 Watt hours.
int userDemand;                          // 0 for no demand, 1 for low demand, 2 for high demand

float renewables_watts;            // in Watt
float batteryCharge_joules;           // in Joules. Describes the current level of battery charge in Watt-hr. 1Wh = 3600J

unsigned long intime;
unsigned long extime;
unsigned long time_to_empty_battery;
char junk = ' ';
const int chargerPin = 2;        //relay for battery chargerPin  
const int pumpPin = 3;          //relay for submersible pump connect to pin 3，Plug into NC
const int valvePin = 7 ;         //relay for solenoid valvePin connect to pin7
const byte  ENA = 6;         //L298N pins setting, for conveyor DC motor. First pin
const byte  INA1 = 5;       // Second pin for motor
const byte  INA2 = 4;       // Third pin for motor
const int waterLevelPin = A3;          //Pin A3 is for the water level sensor. Why integer? Bcos it is given by manufacturer. A3 will be converted to integer on the board.


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
}


void loop()
{ 
  int w0 = analogRead(waterLevelPin);
  Serial.print("Water level(w0) = "); Serial.println(w0);
  batteryCharge_joules = batteryLevel_percent*3600 ;  // current battery capacity here is 180 000 J if battery level = 50%
  Serial.print("Initial battery charge = "); Serial.print(batteryCharge_joules); Serial.println(" Joules")

  // Mode 1:pumps on, conveyor full speed. Renewables share at 100%, low and medium level water supply, high demand. 
  while((renewables_watts>=total_system_power && 1<analogRead(waterLevelPin)<400 && userDemand ==2 ) || (renewables_watts<total_system_power && analogRead(waterLevelPin)<=5 && userDemand ==2) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop = "); Serial.println(intime);

    time_to_empty_battery = (batteryCharge_joules / (total_system_power-renewables_watts)) // Give us time in seconds
    
    int w1=analogRead(waterLevelPin);
    Serial.print("Water Level(w1)=");
    Serial.println(w1);
       
       if(analogRead(waterLevelPin)>401)  // 401 is minimum level to be considered high water supply. Break out from firsst condition
          {
           Serial.print("end of scenerio 1, end water level ="); Serial.println(analogRead(waterLevelPin));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           batteryCharge_joules = batteryCharge_joules + (renewables_watts - total_system_power)*((extime - intime)/1000);    // unit: joule
           batteryLevel_percent = batteryCharge_joules/360000;
           
           if(batteryLevel_percent>=100)
           {
            batteryCharge_joules=360000;   // Battery charge cannot be greater than 360000, ie no greater than 100%
            batteryLevel_percent = 100;
           }
           
           Serial.print("batteryCharge_joules1="); Serial.println(batteryCharge_joules);
           // Print on LCD as well
           break;
          }
          
     
        if(renewables_watts<total_system_power &&(millis()-intime)>=time_to_empty_battery )
        {
         batteryCharge_joules=0;
         batteryLevel_percent=0; 
         Serial.print("end of scenerio 1, Battery is empty, please charge");
         break;
        }
        

    // digitalWrite(chargerPin , HIGH);  // turn on chargerPin  to charge batteries. We do not need to charge for demo.
    digitalWrite(pumpPin, HIGH);    //  turn on submersible pump 
 
    runMotorFast();   delay(2000);
    brakeMotor;  delay(500);
    digitalWrite(valvePin, HIGH);  delay(1000);
    digitalWrite(valvePin, LOW);
    Serial.println("pumps on, conveyor full speed");
   }
  
  /*
  // op 2:pumps on, conveyor half speed
  while((renewables_watts>=100 && analogRead(waterLevelPin)<400 && userDemand ==1 ) || (renewables_watts<100 && analogRead(waterLevelPin)<18 && userDemand ==1) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    time_to_empty_battery =(batteryCharge_joules/(renewables_watts-100))*1000; //[unit:ms], time duration that battery would die
    
    int w2=analogRead(waterLevelPin);
    Serial.print("Water Level(w2)=");
    Serial.println(w2);
       if((analogRead(waterLevelPin)>400))
          {
           Serial.print("end of scenerio 2, end water level=");
           Serial.println(analogRead(waterLevelPin));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           batteryCharge_joules =batteryCharge_joules +(renewables_watts - 100)*((extime - intime)/1000);    // unit: joule
           batteryLevel_percent = batteryCharge_joules/1260; // in %
           if(batteryLevel_percent>=100)
           {
            batteryCharge_joules=126000;
            batteryLevel_percent=100;
           }
           Serial.print("batteryCharge_joules1=");   // 或是print batteryLevel_percent value 
           Serial.println(batteryCharge_joules);
           break;
          }
        if(renewables_watts<100 &&(millis()-intime)>=time_to_empty_battery )
        {
         batteryCharge_joules=0;
         batteryLevel_percent=0; 
         Serial.print("batteryLevel_percent(%)=");   // 或是print batteryLevel_percent value 
         Serial.println(batteryLevel_percent);
         Serial.print("end of scenerio 2, Battery is empty, please charge");
         break;
        }

    digitalWrite(chargerPin , HIGH);  // turn on chargerPin  to charge batteries
    digitalWrite(pumpPin, HIGH);    //  turn on submersible pump 
 
    runMotorSlow();   delay(2000);
    brakeMotor;  delay(2000);
    digitalWrite(valvePin, HIGH);  delay(1000);
    digitalWrite(valvePin, LOW);   delay(1000);
    Serial.println("pumps on, conveyor half speed");
  }
   // op 3: pump on, conveyor off
    while((renewables_watts>=10 && 5<analogRead(waterLevelPin)<400 && userDemand ==0 ) || (renewables_watts<10 && analogRead(waterLevelPin)<5 && userDemand ==0) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    time_to_empty_battery =(batteryCharge_joules/(renewables_watts-100))*1000; //[unit:ms], time duration that battery would die
    
    int w3=analogRead(waterLevelPin);
    Serial.print("Water Level(w3)=");
    Serial.println(w3);
       
       if(analogRead(waterLevelPin)>400)
          {
           Serial.print("end of scenerio 3, end water level=");
           Serial.println(analogRead(waterLevelPin));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           batteryCharge_joules =batteryCharge_joules +(renewables_watts - 10)*((extime - intime)/10);    // unit: joule
           batteryLevel_percent = batteryCharge_joules/10; // in %
           
           if(batteryLevel_percent>=100)
           {
            batteryCharge_joules=126000; // total battery capacity to be installed
            batteryLevel_percent=100;
           }
           
           Serial.print("batteryCharge_joules1=");   // 或是print batteryLevel_percent value 
           Serial.println(batteryCharge_joules);
           break;
          }
          
     
        if(renewables_watts<10 &&(millis()-intime)>=time_to_empty_battery )
        {
         batteryCharge_joules=0;
         batteryLevel_percent=0; 
         Serial.print("end of scenerio 3, Battery is empty, please charge");
         break;
        }
        

    digitalWrite(chargerPin , HIGH);  // turn on chargerPin  to charge batteries
    digitalWrite(pumpPin, HIGH);    //  turn on submersible pump 
 
    stopMotor();   
    digitalWrite(valvePin, HIGH);  delay(1000);
    digitalWrite(valvePin, LOW);   delay(1000);
    Serial.println("pumps on, conveyor off");
   }   
  // op 4: pump off, conveyor full speed
  while((renewables_watts>=10 && analogRead(waterLevelPin)>400 && userDemand ==2 ) || (renewables_watts<10 && analogRead(waterLevelPin)>18 && userDemand ==2) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    time_to_empty_battery =(batteryCharge_joules/(renewables_watts-100))*1000; //[unit:ms], time duration that battery would die
    
    int w4=analogRead(waterLevelPin);
    Serial.print("Water Level(w4)=");
    Serial.println(w4);
       
       if(analogRead(waterLevelPin)<400)
          {
           Serial.print("end of scenerio 4, end water level=");
           Serial.println(analogRead(waterLevelPin));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           batteryCharge_joules =batteryCharge_joules +(renewables_watts - 10)*((extime - intime)/10);    // unit: joule
           batteryLevel_percent = batteryCharge_joules/10; // in %
           
           if(batteryLevel_percent>=100)
           {
            batteryCharge_joules=126000; // total installed battery capacity
            batteryLevel_percent=100;
           }
           
           Serial.print("batteryCharge_joules1=");   // 或是print batteryLevel_percent value 
           Serial.println(batteryCharge_joules);
           break;
          }
          
     
        if(renewables_watts<10 &&(millis()-intime)>=time_to_empty_battery )
        {
         batteryCharge_joules=0;
         batteryLevel_percent=0; 
         Serial.print("end of scenerio 4, Battery is empty, please charge");
         break;
        }
        

    digitalWrite(chargerPin , HIGH);  // turn on chargerPin  to charge batteries
    digitalWrite(pumpPin, LOW);    //  turn on submersible pump 
 
    runMotorFast();   delay(2000);
    brakeMotor;  delay(2000);
    digitalWrite(valvePin, HIGH);  delay(1000);
    digitalWrite(valvePin, LOW);   delay(1000);
    Serial.println("pumps off, conveyor full speed");
   }   
  // op 5: pump off, conveyor half speed
  while((renewables_watts>=10 && analogRead(waterLevelPin)>400 && userDemand ==1 ) || (renewables_watts<10 && 18<analogRead(waterLevelPin)<600 && userDemand ==1) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    time_to_empty_battery =(batteryCharge_joules/(renewables_watts-100))*1000; //[unit:ms], time duration that battery would die
    
    int w5=analogRead(waterLevelPin);
    Serial.print("Water Level(w5)=");
    Serial.println(w5);
       
       if(renewables_watts>=10 && analogRead(waterLevelPin)<400)
          {
           Serial.print("end of scenerio 5, end water level=");
           Serial.println(analogRead(waterLevelPin));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           batteryCharge_joules =batteryCharge_joules +(renewables_watts - 10)*((extime - intime)/10);    // unit: joule
           batteryLevel_percent = batteryCharge_joules/10; // in %
           
           if(batteryLevel_percent>=100)
           {
            batteryCharge_joules=126000; // total installed battery capacity
            batteryLevel_percent=100;
           }
           
           Serial.print("batteryCharge_joules1=");   // 或是print batteryLevel_percent value 
           Serial.println(batteryCharge_joules);
           break;
          }
          
     
        if(renewables_watts<10 &&(millis()-intime)>=time_to_empty_battery )
        {
         batteryCharge_joules=0;
         batteryLevel_percent=0; 
         Serial.print("end of scenerio 5, Battery is empty, please charge");
         break;
        }
        

    //digitalWrite(chargerPin , HIGH);  // !!! turn on chargerPin  to charge batteries
    digitalWrite(pumpPin, LOW);    //  turn on submersible pump 
 
    runMotorSlow();   delay(2000);
    brakeMotor;  delay(2000);
    digitalWrite(valvePin, HIGH);  delay(1000);
    digitalWrite(valvePin, LOW);   delay(1000);
    Serial.println("pumps off, conveyor half speed");
   }  
  // op 6:pumps off, conveyor off
  while((renewables_watts>=10 && analogRead(waterLevelPin)>400 && userDemand ==0 ) || (renewables_watts<10 && 18<analogRead(waterLevelPin)<600 && userDemand ==0) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    time_to_empty_battery =(batteryCharge_joules/(renewables_watts-100))*1000; //[unit:ms], time duration that battery would die
    
    int w6=analogRead(waterLevelPin);
    Serial.print("Water Level(w6)=");
    Serial.println(w6);
       
       if(renewables_watts>=10 && analogRead(waterLevelPin)<400)
          {
           Serial.print("end of scenerio 6, end water level=");
           Serial.println(analogRead(waterLevelPin));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           batteryCharge_joules =batteryCharge_joules +(renewables_watts - 10)*((extime - intime)/10);    // unit: joule
           batteryLevel_percent = batteryCharge_joules/10; // in %
           
           if(batteryLevel_percent>=100)
           {
            batteryCharge_joules=126000; // total installed battery capacity
            batteryLevel_percent=100;
           }
           
           Serial.print("batteryCharge_joules1=");   // 或是print batteryLevel_percent value 
           Serial.println(batteryCharge_joules);
           break;
          }
          
     
        if(renewables_watts<10 &&(millis()-intime)>=time_to_empty_battery )
        {
         batteryCharge_joules=0;
         batteryLevel_percent=0; 
         Serial.print("end of scenerio 6, Battery is empty, please charge");
         break;
        }
        

    //digitalWrite(chargerPin , HIGH);  // !!! turn on chargerPin  to charge batteries
    digitalWrite(pumpPin, LOW);    //  turn on submersible pump 
 
    stopMotor();   
    digitalWrite(valvePin, LOW);   delay(1000);
    Serial.println("pumps off, conveyor off");
   }  
*/
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

//====================================
  
