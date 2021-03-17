#pragma once

#include <Servo.h>

class servo
{
  private:
    Servo s;

    float inc = 1;
    unsigned long lastTime = 0 , timeDur;
  public:

    servo(int pin)
    {
      s.attach(pin);
    }

    //turns the servo to the specified angle(degrees) with the specified speed(degrees per second)
    float currentPos = 0;
    bool turnTo(float dest, float spd)// ? degree per seconds
    {
      //calculates the time it takes for each loop to adjust the speed of the servo
      timeDur = micros() - lastTime;
      lastTime = micros();
      float increment = spd * timeDur / 1000000;

      if (currentPos < dest)
        currentPos += increment;
      else if (currentPos > dest)
        currentPos -= increment;

      if (abs(currentPos - dest) <= increment)
      {
        s.write(dest);
        return true;
      }

      s.write(currentPos);
      return false;
    }

   //turns the motor at the specified speed(degrees per second) between specified angles(degrees)
    int pole = 1;
    void sweepBetween(int from, int to, float spd)
    {
      if (pole == 1)
      {
        if (turnTo(from, spd))
          pole = -1;
      }
      else if (pole == -1)
      {
        if (turnTo(to, spd))
          pole = 1;
      }

    }
};

