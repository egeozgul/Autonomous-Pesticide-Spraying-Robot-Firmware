#include "Configuration.h"
#include "RMotor.h"
#include "RServo.h"
#include "RUltrasonicSensor.h"
#include "RPump.h"

pump *Pump;
servo *Head;
motor *motor_FR;
motor *motor_FL; 
motor *motor_RR;
motor *motor_RL;
ultrasonicSensor *sensor;

void setup()
{
  Serial.begin(9600);
  
  Pump = new pump(pumpPin_pwm,pumpPin_dir);
  Head = new servo(headServo_Pin);
  sensor = new ultrasonicSensor(pingPin);
  motor_FR = new motor(MFR_pwm_Pin, MFR_dir_Pin, MFR_encd_A, MFR_encd_B, 4.8, true);
  motor_FL = new motor(MFL_pwm_Pin, MFL_dir_Pin, MFL_encd_A, MFL_encd_B, 4.8, false);
  motor_RR = new motor(MRR_pwm_Pin, MRR_dir_Pin, MRR_encd_A, MRR_encd_B, 4.8, true);
  motor_RL = new motor(MRL_pwm_Pin, MRL_dir_Pin, MRL_encd_A, MRL_encd_B, 4.8, false);

  
  
  pinMode(pin_button1,INPUT);
  pinMode(pin_button2,INPUT);
  
  pinMode(pin_power,OUTPUT);
  pinMode(pin_gnd,OUTPUT);
  
  digitalWrite(pin_power,HIGH);
  digitalWrite(pin_gnd,LOW);  

  pinMode(pin_led1,OUTPUT);
  pinMode(pin_led2,OUTPUT);
}

int pot1 = 0, pot2 = 0;
int button1 = LOW, button2 = LOW; 

int sprinkle_enabled = false;

void loop()
{
  
  sprinkle_enabled = digitalRead(pin_button1);

  float ff = (float)(2.0f*pot1/1024.0f);

  if(sprinkle_enabled == HIGH)
  {
  digitalWrite(pin_led1,HIGH);
  digitalWrite(pin_led2,HIGH);
  }
  else if(sprinkle_enabled == LOW)
  {
  digitalWrite(pin_led1,LOW);
  digitalWrite(pin_led2,LOW);
  }
  

  motor_FL->walkBy(ff);
  motor_FR->walkBy(ff);
  motor_RL->walkBy(ff);
  motor_RR->walkBy(ff);
 
  motor_FL->updateMotor(240);
  motor_FR->updateMotor(240);
  //motor_RR->updateMotor(240);
  //motor_RL->updateMotor(240);
}

