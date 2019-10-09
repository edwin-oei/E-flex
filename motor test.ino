#include "time.h"
#include "math.h"               // include the Math Library
#include<Wire.h>

const byte  ENA = 6;         //L298N pins setting, for conveyor DC motor. First pin
const byte  INA1 = 5;       // Second pin for motor
const byte  INA2 = 4;       // Third pin for motor

void setup(){
  Serial.begin(9600);
  Serial.flush();
}


void loop(){
  runMotorFast(); delay(6000);
  brakeMotor(); delay(5000);
  runMotorSlow(): delay(6000);
  brakeMotor(); delay(5000);
}




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
