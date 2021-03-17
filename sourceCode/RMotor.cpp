#include "RMotor.h"
//@speed -> -255 to 255 
void motor::runMotor(int Speed)
{
  //check if the motor is reversed
  if (Speed == motorCurrSpeed)
    return;

  if (Speed > 0)
  {
    if (motorDir != LOW)
    {
      digitalWrite(mDir_Pin, LOW);
      motorDir = LOW;
    }
    analogWrite(mPwm_Pin, Speed);
  }
  else if (Speed == 0)
    analogWrite(mPwm_Pin, 0);
  else
  {
    if (motorDir != HIGH)
    {
      digitalWrite(mDir_Pin, HIGH);
      motorDir = HIGH;
    }
    analogWrite(mPwm_Pin, -Speed);
  }

  return;
}

motor::motor(int pin_Pwm, int pin_mDir, int pin_enc_A, int pin_enc_B , float wheelRad, bool isForward)
{
  mPwm_Pin = pin_Pwm;
  mDir_Pin = pin_mDir;
  encA_Pin = pin_enc_A;
  encB_Pin = pin_enc_B;
  wheeleRadius = wheelRad;
  calibrator = isForward;
  
  pinMode(mPwm_Pin, OUTPUT);
  pinMode(mDir_Pin, OUTPUT);
  motorCurrSpeed = 0;

  digitalWrite(mDir_Pin, motorDir);
  analogWrite(mPwm_Pin, 0);
  
  encdr = new Encoder(encA_Pin, encB_Pin);
  encdr->write(0); // zeros the encoder
  return;
}


bool motor::updateMotor(int maxCurr = 230)
{
  maxCurrent = maxCurr;
  frameDuration = micros() - Time;
  Time = micros();

  currentPos = encdr->read();

  diff = currentPos - destAngle;

   #ifdef CallibrateMotors
    float fr = (float)((float)analogRead(8)/1024.0f);//1.015
    fr = 1.0f + fr/30.0f;
    double value = (float)maxCurr*2.0f / (1.0f + (float)pow( fr,1+ diff)) - (float)maxCurr;
  
  
   //#ifdef LogCallibrationVal
    //Serial.println(fr*10000);//1.0205f
    
   #else
    double value = (float)maxCurr*2.0f / (1.0f + (float)pow( val_Sigmoid,1+ diff)) - (float)maxCurr;
    #ifdef LogCallibrationVal
      Serial.println(val_Sigmoid*10000);//1.0205f
     #endif
   #endif
   
   runMotor(value);

  if (diff > 250)
    return false;
  
  return true;
}


//updates the destAngle according to user input and frame duration, so that the by calling updateMotor function the motor can reach to the destAngle
//the same principle is applied in related functions below which are turnTo, turnBy, walkTo, walkBy functions.
//turns the motor untill it rotates exactly "turns" number of turns
//must be called every loop to update the new currentPosition of the motor
//number of turns to be rotated, the speed in terms of rotation per second
bool motor::turnTo(double turns, double rps = 0.5)// r/s
{
    if(calibrator)
      turns = -turns;
      
    if (destAngle < (long)(0.5+turns * 12000.0))
    {
      tempVal1 += (0.012f * abs(rps) * (double)frameDuration) ;
      destAngle = (long)round(tempVal1);
      return false;
    }
    else if (destAngle > (long)round(turns * 12000.0))
    {
      tempVal1 -= (0.012f * abs(rps) * (double)frameDuration) ;
      destAngle = (long)round(tempVal1);
      return false;
    }
    else
      return true;
}

//rotates while this function is called every loop, if it is not called the motor stops
//roation per second
void motor::turnBy(double rps = 0.5f)// r/s
{
  if(calibrator)
      rps = -rps;
      
  tempVal2 += (0.012f * rps * (double)frameDuration) ;
  destAngle = (long)round(tempVal2);
  return;
}

//same with turnTo, but in meters radius to the calculation  
bool motor::walkTo(double distance, double spd = 0.1)
{      
  return turnTo( distance / (double)(wheeleRadius * 2 * PI/100), spd / (double)(wheeleRadius * 2 * PI/100));
}

//same with turnBy, but in meters radius to the calculation  
void motor::walkBy(double spd = 0.1)
{      
  turnBy(spd / (double)(wheeleRadius * 2 * PI/100));
  return;
}

//resets the motors and zeros everything
void motor::reset()
{
  encdr->write(0);
  destAngle = 0;
  tempVal1 = 0;
  tempVal2 = 0;
  return;
} 

//deconstructor for the motor, frees encoder object
motor::~motor()
{
  delete encdr;
  return;
}
