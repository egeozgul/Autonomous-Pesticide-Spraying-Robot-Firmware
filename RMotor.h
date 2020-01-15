#pragma once
#include "Configuration.h"
#include <Encoder.h>
#include <math.h>

class motor
{
  private:    
    Encoder* encdr = NULL;

        //setup variables
    int mPwm_Pin, mDir_Pin;
    int encA_Pin, encB_Pin;
    float wheeleRadius; // in centimeters
    int maxCurrent=255;     //(0 to 255) maximum percentage of current to be applied 
    bool calibrator;    //used to reverse the motor's default, needed for reversing one side of the robot's motors

        //updated variables
    long frameDuration;                                     //how many microseconds each loop takes is stored here
    long destAngle = 0;                                     // the value of encoder that must be catched up by the motor
    
    int motorCurrSpeed = 0;
    bool motorDir = HIGH;
    void runMotor(int Speed);                               //drive the motor freely without encoder with the specified speed, but it is private because not used by the user

  public:
    motor(int pin_Pwm, int pin_mDir, int pin_enc_A, int pin_enc_B , float wheelRad, bool isForward = true);     //pin_pwm, pin_dir, pin_encoder_channel1, pin_encoder_channel2, wheele_radius(cm)

            double tempVal1 = 0;
    bool turnTo(double turns, double rps = 0.5f);           //prepares the motor to turn by specified amount of turns (round per second)
            double tempVal2 = 0;
    void turnBy(double rps = 0.5);                          //prepares the motor to rotate at the given rps

    bool walkTo(double distance, double spd = 0.1);         //prepares the motor to move the wheel by the specified distance(meters) and speed(meters/second)
    void walkBy(double spd = 0.1);                          //prepares the motor to move the wheel at the specified speed(meters /second)

    void reset();                                      //maked the current angle of the motor 0

    double diff;
    long currentPos, Time = 0;
    bool updateMotor(int maxCurrent = 230);                 //keeps the motor's angle at the proper angle,must be called each loop in order to update the motor's currentPosition
    ~motor();
};



