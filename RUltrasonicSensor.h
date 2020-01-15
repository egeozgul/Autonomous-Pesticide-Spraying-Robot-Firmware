#pragma once

class ultrasonicSensor
{
  private:
    int ultra_pin;
    
    long duration, cm, inches;
  public:
    ultrasonicSensor(int pin)
    {
      ultra_pin = pin;
    }
    
    //returns the distance in cm
    
    long senseDistance_cm()
    {
      pinMode(ultra_pin, OUTPUT);
      digitalWrite(ultra_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(ultra_pin, HIGH);
      delayMicroseconds(5);
      digitalWrite(ultra_pin, LOW);

      pinMode(ultra_pin, INPUT);
      duration = pulseIn(ultra_pin, HIGH);
      return duration / 29 / 2;
    }
    //returns the distance in inch
    long senseDistance_inch()
    {
      pinMode(ultra_pin, OUTPUT);
      digitalWrite(ultra_pin, LOW);
      delayMicroseconds(2);
      digitalWrite(ultra_pin, HIGH);
      delayMicroseconds(5);
      digitalWrite(ultra_pin, LOW);

      pinMode(ultra_pin, INPUT);
      duration = pulseIn(ultra_pin, HIGH);
      return duration / 74 / 2;
    }
};
