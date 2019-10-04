// 2019.10.03
// Power required to operate of the whole system = 100W

#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

float wind_power;                         // in Watt
int wind_power_share;                     // in %
float solar_power;                        // in Watt
int solar_power_share;                    // in %
int charge_state = 0 ;                    // initial 18650 state of charge in %
int user_demand;                          // 0 for no demand, 1 for low demand, 2 for high demand



float RE_power;            // in Watt
float charge_stateControlValue;           // in Watt-hr or mAh to convert to charge_state !!! per cell= 11.7Wh, relative cap.=3250mAh, take 1C charge and discharge rate, voltage 3.6V

unsigned long intime;               //  1 Wh= 3600J
unsigned long extime;
unsigned long dischargetime;
char junk = ' ';
const int CHARGER = 2;        //relay for battery CHARGER  
const int SPUMP = 3;          //relay for submersible pump connect to pin3，Plug into NC
const int VALVE = 7 ;         //relay for solenoid VALVE connect to pin7
const byte  ENA = 6;         //L298N pins setting, for conveyor DC motor
const byte  INA1 =5;
const byte  INA2 =4; 
const int water_level = A3;          //Pin A3 is for the water level sensor.


void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);           // set up Serial library at 9600 bps. This is the standard value.
  Serial.println("");
  Serial.flush();
  pinMode(water_level, INPUT);  // This pin is configured to be an input pin
  pinMode(CHARGER, OUTPUT);     //// This pin is configured to be an output pin
  pinMode(SPUMP, OUTPUT);
  pinMode(VALVE, OUTPUT);
  pinMode(INA1, OUTPUT);
  pinMode(INA2, OUTPUT);
  pinMode(ENA, OUTPUT);

  
  lcd.begin(16, 2);
  // initialization of LCD
  lcd.setCursor(0, 0); // cursor at first row設定游標位置在第一行行首
  lcd.print("E-Flex Demo");
  delay(1000);
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("Let's start!");
  delay(2000);
  lcd.clear(); //顯示清除
  
  // Serial input of wind value ,  solar value , customer demand
  Serial.println("Enter your demand for water here, from 0(no demand), 1(low demand), 2(high demand), Press ENTER");
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character
  {
      // input of user demand
    user_demand = Serial.parseInt();        // new command in 1.0 forward
    Serial.print("user_demand = "); Serial.println(user_demand, DEC);
    while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters
    { junk = Serial.read() ; }      // clear the keyboard buffer
  }
  Serial.println("Enter in % for wind, Press ENTER");
  while (Serial.available() == 0) ;  // Wait here until input buffer has a character
  {
      // % coming from wind power
    wind_power_share = Serial.parseInt();        // new command in 1.0 forward
    Serial.print("wind_power_share = "); Serial.println(wind_power_share, DEC);
    //int w= wind_power_share;
    //lcd.setCursor(0,0);          
    //lcd.print("%wind=");   lcd.print(w); 
    while (Serial.available() > 0)  // .parseFloat() can leave non-numeric characters
    { junk = Serial.read() ; }      // clear the keyboard buffer
  }

  Serial.println("Enter in % for solar, Press ENTER");
  while (Serial.available() == 0) ;
  {
      //% coming from solar power
    solar_power_share = Serial.parseInt();
    Serial.print("solar_power_share = "); Serial.println(solar_power_share, DEC);
    //int s= solar_power_share;
    //lcd.setCursor(0,1);          
    //lcd.print("%solar=");   lcd.print(s);    
    while (Serial.available() > 0)
    { junk = Serial.read() ; }
    Serial.println();
    wind_power= wind_power_share*0.1; // times a number to change % into watt, if wind=100%= 60W
    Serial.print("wind_power = "); Serial.println(wind_power, DEC);
    solar_power= solar_power_share*0.1;// times a number to change % into watt, if solar=100%= 120W
    Serial.print("solar_power = "); Serial.println(solar_power, DEC);
    RE_power= wind_power + solar_power;
    Serial.print("RE_power = "); Serial.println(RE_power, DEC);
    int w= wind_power;
    int s= solar_power;
    int r=RE_power;
    lcd.setCursor(0,0);          
    lcd.print("wind(W)=");    lcd.print(w); 
    lcd.print("solar(W)=");   lcd.print(s);
    delay(2000);              lcd.clear(); 
    lcd.setCursor(0,0);
    lcd.print("RE mix(W)=");  lcd.print(r);
  }
}
/*
void loop()
{ int w0=analogRead(waterLevel);
  Serial.print("Water Level(w0)=");
  Serial.println(w0);
  charge_stateControlValue= charge_state*0.1 ;
  Serial.print("initial charge_stateControlValue=");   // should be a designed value by us
  Serial.println(charge_stateControlValue);
   
  while( (RE_power>10 && 5<analogRead(waterLevel)<400)  )      //少一個SOC的情況，考慮水位
  { 
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);
    
    int w1=analogRead(waterLevel);
    Serial.print("Water Level(w1)=");
    Serial.println(w1);
       if((analogRead(waterLevel)>400))
          {
           Serial.print("end of scenerio 1, end water level=");
           Serial.println(analogRead(waterLevel));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           charge_stateControlValue =charge_stateControlValue +(RE_power - 10)*((extime - intime)/1);    // 確定capacity後要過衝保護，注意時間單位watt-hr or watt-sec
           charge_state = charge_stateControlValue*10; // to be defined later
           Serial.print("charge_stateControlValue1=");   // 或是print charge_state value 
           Serial.println(charge_stateControlValue);
           break;
          }

    digitalWrite(CHARGER , HIGH);  // turn on CHARGER  to charge batteries
    digitalWrite(SPUMP, HIGH);    //  turn on submersible pump 
    dcMHIGH();   delay(3000);
    dcMBrake();  delay(2000);
    Serial.println("pumps on, conveyor full speed");
    
  }
*/

void loop()
{ int w0=analogRead(waterLevel);
  Serial.print("Water Level(w0)=");
  Serial.println(w0);
  charge_stateControlValue= charge_state*0.1 ;  // [unit:Joule] estimate: per Li-ion is 11.7Wh(=42120J), we have 3 in parallel, total storage capacity=35Wh(=126000J)
  Serial.print("initial charge_stateControlValue=");   // should be a designed value by us
  Serial.println(charge_stateControlValue);

  // op 1:pumps on, conveyor full speed
  while((RE_power>=10 && 1<analogRead(waterLevel)<400 && user_demand ==2 ) || (RE_power<10 && analogRead(waterLevel)<=5 && user_demand ==2) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    dischargetime =(charge_stateControlValue/(RE_power-100))*1000; //[unit:ms], time duration that battery would die
    
    int w1=analogRead(waterLevel);
    Serial.print("Water Level(w1)=");
    Serial.println(w1);
       
       if(analogRead(waterLevel)>401)
          {
           Serial.print("end of scenerio 1, end water level=");
           Serial.println(analogRead(waterLevel));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           charge_stateControlValue =charge_stateControlValue +(RE_power - 10)*((extime - intime)/10);    // unit: joule
           charge_state = charge_stateControlValue/10; // in %
           
           if(charge_state>=100)
           {
            charge_stateControlValue=126000;
            charge_state=100;
           }
           
           Serial.print("charge_stateControlValue1=");   // 或是print charge_state value 
           Serial.println(charge_stateControlValue);
           break;
          }
          
     
        if(RE_power<10 &&(millis()-intime)>=dischargetime )
        {
         charge_stateControlValue=0;
         charge_state=0; 
         Serial.print("end of scenerio 1, Battery is empty, please charge");
         break;
        }
        

    digitalWrite(CHARGER , HIGH);  // turn on CHARGER  to charge batteries
    digitalWrite(SPUMP, HIGH);    //  turn on submersible pump 
 
    dcMHIGH();   delay(2000);
    dcMBrake();  delay(2000);
    digitalWrite(VALVE, HIGH);  delay(1000);
    digitalWrite(VALVE, LOW);   delay(1000);
    Serial.println("pumps on, conveyor full speed");
   }
  
  /*
  // op 2:pumps on, conveyor half speed
  while((RE_power>=100 && analogRead(waterLevel)<400 && user_demand ==1 ) || (RE_power<100 && analogRead(waterLevel)<18 && user_demand ==1) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    dischargetime =(charge_stateControlValue/(RE_power-100))*1000; //[unit:ms], time duration that battery would die
    
    int w2=analogRead(waterLevel);
    Serial.print("Water Level(w2)=");
    Serial.println(w2);
       if((analogRead(waterLevel)>400))
          {
           Serial.print("end of scenerio 2, end water level=");
           Serial.println(analogRead(waterLevel));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           charge_stateControlValue =charge_stateControlValue +(RE_power - 100)*((extime - intime)/1000);    // unit: joule
           charge_state = charge_stateControlValue/1260; // in %
           if(charge_state>=100)
           {
            charge_stateControlValue=126000;
            charge_state=100;
           }
           Serial.print("charge_stateControlValue1=");   // 或是print charge_state value 
           Serial.println(charge_stateControlValue);
           break;
          }
        if(RE_power<100 &&(millis()-intime)>=dischargetime )
        {
         charge_stateControlValue=0;
         charge_state=0; 
         Serial.print("charge_state(%)=");   // 或是print charge_state value 
         Serial.println(charge_state);
         Serial.print("end of scenerio 2, Battery is empty, please charge");
         break;
        }

    digitalWrite(CHARGER , HIGH);  // turn on CHARGER  to charge batteries
    digitalWrite(SPUMP, HIGH);    //  turn on submersible pump 
 
    dcMLOW();   delay(2000);
    dcMBrake();  delay(2000);
    digitalWrite(VALVE, HIGH);  delay(1000);
    digitalWrite(VALVE, LOW);   delay(1000);
    Serial.println("pumps on, conveyor half speed");
  }
   // op 3: pump on, conveyor off
    while((RE_power>=10 && 5<analogRead(waterLevel)<400 && user_demand ==0 ) || (RE_power<10 && analogRead(waterLevel)<5 && user_demand ==0) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    dischargetime =(charge_stateControlValue/(RE_power-100))*1000; //[unit:ms], time duration that battery would die
    
    int w3=analogRead(waterLevel);
    Serial.print("Water Level(w3)=");
    Serial.println(w3);
       
       if(analogRead(waterLevel)>400)
          {
           Serial.print("end of scenerio 3, end water level=");
           Serial.println(analogRead(waterLevel));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           charge_stateControlValue =charge_stateControlValue +(RE_power - 10)*((extime - intime)/10);    // unit: joule
           charge_state = charge_stateControlValue/10; // in %
           
           if(charge_state>=100)
           {
            charge_stateControlValue=126000; // total battery capacity to be installed
            charge_state=100;
           }
           
           Serial.print("charge_stateControlValue1=");   // 或是print charge_state value 
           Serial.println(charge_stateControlValue);
           break;
          }
          
     
        if(RE_power<10 &&(millis()-intime)>=dischargetime )
        {
         charge_stateControlValue=0;
         charge_state=0; 
         Serial.print("end of scenerio 3, Battery is empty, please charge");
         break;
        }
        

    digitalWrite(CHARGER , HIGH);  // turn on CHARGER  to charge batteries
    digitalWrite(SPUMP, HIGH);    //  turn on submersible pump 
 
    dcMStop();   
    digitalWrite(VALVE, HIGH);  delay(1000);
    digitalWrite(VALVE, LOW);   delay(1000);
    Serial.println("pumps on, conveyor off");
   }   
  // op 4: pump off, conveyor full speed
  while((RE_power>=10 && analogRead(waterLevel)>400 && user_demand ==2 ) || (RE_power<10 && analogRead(waterLevel)>18 && user_demand ==2) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    dischargetime =(charge_stateControlValue/(RE_power-100))*1000; //[unit:ms], time duration that battery would die
    
    int w4=analogRead(waterLevel);
    Serial.print("Water Level(w4)=");
    Serial.println(w4);
       
       if(analogRead(waterLevel)<400)
          {
           Serial.print("end of scenerio 4, end water level=");
           Serial.println(analogRead(waterLevel));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           charge_stateControlValue =charge_stateControlValue +(RE_power - 10)*((extime - intime)/10);    // unit: joule
           charge_state = charge_stateControlValue/10; // in %
           
           if(charge_state>=100)
           {
            charge_stateControlValue=126000; // total installed battery capacity
            charge_state=100;
           }
           
           Serial.print("charge_stateControlValue1=");   // 或是print charge_state value 
           Serial.println(charge_stateControlValue);
           break;
          }
          
     
        if(RE_power<10 &&(millis()-intime)>=dischargetime )
        {
         charge_stateControlValue=0;
         charge_state=0; 
         Serial.print("end of scenerio 4, Battery is empty, please charge");
         break;
        }
        

    digitalWrite(CHARGER , HIGH);  // turn on CHARGER  to charge batteries
    digitalWrite(SPUMP, LOW);    //  turn on submersible pump 
 
    dcMHIGH();   delay(2000);
    dcMBrake();  delay(2000);
    digitalWrite(VALVE, HIGH);  delay(1000);
    digitalWrite(VALVE, LOW);   delay(1000);
    Serial.println("pumps off, conveyor full speed");
   }   
  // op 5: pump off, conveyor half speed
  while((RE_power>=10 && analogRead(waterLevel)>400 && user_demand ==1 ) || (RE_power<10 && 18<analogRead(waterLevel)<600 && user_demand ==1) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    dischargetime =(charge_stateControlValue/(RE_power-100))*1000; //[unit:ms], time duration that battery would die
    
    int w5=analogRead(waterLevel);
    Serial.print("Water Level(w5)=");
    Serial.println(w5);
       
       if(RE_power>=10 && analogRead(waterLevel)<400)
          {
           Serial.print("end of scenerio 5, end water level=");
           Serial.println(analogRead(waterLevel));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           charge_stateControlValue =charge_stateControlValue +(RE_power - 10)*((extime - intime)/10);    // unit: joule
           charge_state = charge_stateControlValue/10; // in %
           
           if(charge_state>=100)
           {
            charge_stateControlValue=126000; // total installed battery capacity
            charge_state=100;
           }
           
           Serial.print("charge_stateControlValue1=");   // 或是print charge_state value 
           Serial.println(charge_stateControlValue);
           break;
          }
          
     
        if(RE_power<10 &&(millis()-intime)>=dischargetime )
        {
         charge_stateControlValue=0;
         charge_state=0; 
         Serial.print("end of scenerio 5, Battery is empty, please charge");
         break;
        }
        

    //digitalWrite(CHARGER , HIGH);  // !!! turn on CHARGER  to charge batteries
    digitalWrite(SPUMP, LOW);    //  turn on submersible pump 
 
    dcMLOW();   delay(2000);
    dcMBrake();  delay(2000);
    digitalWrite(VALVE, HIGH);  delay(1000);
    digitalWrite(VALVE, LOW);   delay(1000);
    Serial.println("pumps off, conveyor half speed");
   }  
  // op 6:pumps off, conveyor off
  while((RE_power>=10 && analogRead(waterLevel)>400 && user_demand ==0 ) || (RE_power<10 && 18<analogRead(waterLevel)<600 && user_demand ==0) )
  {
    intime = millis();            // time that enter this while loop
    Serial.print("time that enter this while loop=");
    Serial.println(intime);

    dischargetime =(charge_stateControlValue/(RE_power-100))*1000; //[unit:ms], time duration that battery would die
    
    int w6=analogRead(waterLevel);
    Serial.print("Water Level(w6)=");
    Serial.println(w6);
       
       if(RE_power>=10 && analogRead(waterLevel)<400)
          {
           Serial.print("end of scenerio 6, end water level=");
           Serial.println(analogRead(waterLevel));
           extime = millis();
           Serial.print("time that exit this while loop=");
           Serial.println(extime);
           charge_stateControlValue =charge_stateControlValue +(RE_power - 10)*((extime - intime)/10);    // unit: joule
           charge_state = charge_stateControlValue/10; // in %
           
           if(charge_state>=100)
           {
            charge_stateControlValue=126000; // total installed battery capacity
            charge_state=100;
           }
           
           Serial.print("charge_stateControlValue1=");   // 或是print charge_state value 
           Serial.println(charge_stateControlValue);
           break;
          }
          
     
        if(RE_power<10 &&(millis()-intime)>=dischargetime )
        {
         charge_stateControlValue=0;
         charge_state=0; 
         Serial.print("end of scenerio 6, Battery is empty, please charge");
         break;
        }
        

    //digitalWrite(CHARGER , HIGH);  // !!! turn on CHARGER  to charge batteries
    digitalWrite(SPUMP, LOW);    //  turn on submersible pump 
 
    dcMStop();   
    digitalWrite(VALVE, LOW);   delay(1000);
    Serial.println("pumps off, conveyor off");
   }  
*/
 }




//====================================
//start DC motor of conveyor with high speed-clockwise
void dcMHIGH()
{
 analogWrite(ENA,255);  
 digitalWrite(INA1,HIGH);
 digitalWrite(INA2,LOW);  
}
//start DC motor of conveyor with low speed-clockwise
void dcMLOW()
{
 analogWrite(ENA,128);  
 digitalWrite(INA1,HIGH);
 digitalWrite(INA2,LOW);  
}
//brake DC motor of conveyor
void dcMBrake()
{
 analogWrite(ENA,255);  
 digitalWrite(INA1,LOW);      //LOW OR HIGH
 digitalWrite(INA2,LOW);      //LOW OR HIGH
}

//stop DC motor of conveyor
void dcMStop()
{
  analogWrite(ENA,0);
}

//====================================
  
