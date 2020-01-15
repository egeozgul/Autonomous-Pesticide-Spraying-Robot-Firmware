#pragma once

class pump
{
  private:
    int pin_pwm,pin_dir;
    
  public:
    pump(int pwm_pin, int dir_pin, bool reverse = false)
    {
      pin_pwm = pwm_pin;
      pin_dir = dir_pin;
      pinMode(pwm_pin, OUTPUT);
      pinMode(dir_pin, OUTPUT);
      
      if(reverse)
        digitalWrite(dir_pin, HIGH);
      else
        digitalWrite(dir_pin, LOW);
        
      return;
    }

    void spray(int s)
    {
      analogWrite(pin_pwm, s);
      return;
    }
};
