#include <Encoder.h>

//******USER PREFERENCES*********//
//timing settings
#define TimeToWaitBeforeSpraying  1 //seconds
#define SprayingTime              2 //seconds
#define TimeToWaitAfterSpraying   1 //seconds
#define MovingTimeAfterSpray      2 //seconds 

//distance settings
#define PlantDetectionDistance    25  //cm
#define FrontSensorObjectDistance 35  //cm
#define FrontSensorCheckFrquency   5 //every 5 loops not in Hertz
#define RightSensorCheckFrquency   5 //in Hertz

//speed settings
#define CRAWLING_SPEED_MULTIPLIER 0.5f;

//******IN DEPTH PREFERENCES*********//
#define maxMOTOR_PWM 255       // 0 to 255
#define pumpSprayStrength 255   // 0 to 255

#define sigmoid_coefficient 1.0190f     //motor calibration value- DON'T CHANGE THIS

//*******console pins ******//
#define pin_power 38
#define pin_gnd 36

#define pin_led1 40
#define pin_led2 42
#define pin_button1 32
#define pin_button2 34
#define pin_pot1 5
#define pin_pot2 4

//******ultrasonic Sensor pin setup****//
#define FtrigPin 45
#define FechoPin 43
#define FVcc_Pin 47
#define FGnd_Pin 41

#define StrigPin 35
#define SechoPin 37
#define SVcc_Pin 33
#define SGnd_Pin 39

//******* PUMP pin SETUP *********//
#define pumpPin_pwm 44
#define pumpPin_gnd 51

//******* Motor pin setups********//
#define MFR_pwm_Pin  11
#define MFR_dir_Pin  13
#define MFR_encd_A   20
#define MFR_encd_B   24

#define MFL_pwm_Pin  3 //broken !
#define MFL_dir_Pin  12 //broken !
#define MFL_encd_A   19
#define MFL_encd_B   26

#define MRL_pwm_Pin  6
#define MRL_dir_Pin  4
#define MRL_encd_A   18
#define MRL_encd_B   28

#define MRR_pwm_Pin  7
#define MRR_dir_Pin  5
#define MRR_encd_A   21
#define MRR_encd_B   22

//Macro's for motor drive functions
#define driveFR(a)  drive(-a,&MFR_FollowAngle, ENFR->read(),frameDuration,MFR_dir_Pin,MFR_pwm_Pin)
#define driveFL(a)  drive(-a,&MFL_FollowAngle,-ENFL->read(),frameDuration,MFL_dir_Pin,MFL_pwm_Pin)
#define driveRR(a)  drive(-a,&MRR_FollowAngle, ENRR->read(),frameDuration,MRR_dir_Pin,MRR_pwm_Pin)
#define driveRL(a)  drive( a,&MRL_FollowAngle, ENRL->read(),frameDuration,MRL_dir_Pin,MRL_pwm_Pin)

//motor encoder pointers
Encoder* ENFR = NULL;
Encoder* ENFL = NULL;
Encoder* ENRR = NULL;
Encoder* ENRL = NULL;

long MFR_FollowAngle = 0;
long MFL_FollowAngle = 0;
long MRR_FollowAngle = 0;
long MRL_FollowAngle = 0;

void setup() 
{
  //***************PUMP SETUP **************
  pinMode(pumpPin_pwm,OUTPUT);
  analogWrite(pumpPin_pwm,0);
  
  pinMode(pumpPin_gnd,OUTPUT);
  digitalWrite(pumpPin_gnd,HIGH);
  
  
  //***************MOTOR SETUP**********
  pinMode(MFR_pwm_Pin,OUTPUT);
  pinMode(MFR_dir_Pin,OUTPUT);
  analogWrite(MFR_pwm_Pin,0);
  
  pinMode(MFL_pwm_Pin,OUTPUT);
  pinMode(MFL_dir_Pin,OUTPUT);
  analogWrite(MFL_pwm_Pin,0);
  
  pinMode(MRR_pwm_Pin,OUTPUT);
  pinMode(MRR_dir_Pin,OUTPUT);
  analogWrite(MRR_pwm_Pin,0);
  
  pinMode(MRL_pwm_Pin,OUTPUT);
  pinMode(MRL_dir_Pin,OUTPUT);
  analogWrite(MRL_pwm_Pin,0);

  //************ULTRASONIC SETUP***************
  pinMode(FtrigPin, OUTPUT);
  pinMode(FechoPin, INPUT);    
  pinMode(FVcc_Pin, OUTPUT);
  pinMode(FGnd_Pin, OUTPUT);
  digitalWrite(FVcc_Pin,HIGH);
  digitalWrite(FGnd_Pin,LOW); 

  pinMode(StrigPin, OUTPUT);
  pinMode(SechoPin, INPUT);
  pinMode(SVcc_Pin, OUTPUT);
  pinMode(SGnd_Pin, OUTPUT);
  digitalWrite(SVcc_Pin,HIGH);
  digitalWrite(SGnd_Pin,LOW); 

  //**************CONSOLE SETUP****************
  pinMode(pin_power,OUTPUT);
  pinMode(pin_gnd,OUTPUT);
  digitalWrite(pin_power,HIGH);
  digitalWrite(pin_gnd,LOW);

  pinMode(pin_led1,OUTPUT);
  pinMode(pin_led2,OUTPUT);
  pinMode(pin_button1,INPUT);
  pinMode(pin_button2,INPUT);
  
  //************ENCODER SETUP******************
  ENFR = new Encoder(MFR_encd_A, MFR_encd_B);
  ENFL = new Encoder(MFL_encd_A, MFL_encd_B);
  ENRR = new Encoder(MRR_encd_A, MRR_encd_B);
  ENRL = new Encoder(MRL_encd_A, MRL_encd_B);
  
  //zero the encoders
  ENFR->write(0);
  ENFL->write(0);
  ENRR->write(0);
  ENRL->write(0);

  //********WAIT FOR 2 SECONDS**********
  delay(2000);
  Serial.begin(9600);
}

//used for timimng frames
long BeginTimer = 0; 

//motor drive variables
double aa=0,bb=0,cc=0,dd=0; 

//timer variable used while spraying
long delayTimeCatch = -1; 

// the state of the robot
enum state{spraying, moving,stopping}RobotState = moving;

float movingSpeed=0;
bool frontSensor,rightSensor;
bool Stop = false;
bool Spraying = false;

bool sprayDisabled = false;
long buttonCounter = 0;
void loop() 
{
  //MEASURE THE FRAME DURATION
  long frameDuration = micros() - BeginTimer;
  BeginTimer = micros();  

  //BUTTON TIMING 
  int button2 = digitalRead(pin_button2);
  if(button2 == HIGH && (millis()-buttonCounter) > 400)
  {
    buttonCounter= millis();
    sprayDisabled = !sprayDisabled;
  }

  if(sprayDisabled)
      digitalWrite(pin_led2,LOW);
    else
      digitalWrite(pin_led2,HIGH);
  
  //SPRAYIN ACTION
  if(Spraying)
  {
      //INITIALIZE THE TIMER IF IT IS NOT ALREADY
      if(delayTimeCatch == -1)
        delayTimeCatch = millis();

      //TIME STATES TO DECIDE THE ACTION OF THE ROBOT 
      long difference = millis() - delayTimeCatch;
      if((difference > TimeToWaitBeforeSpraying*1000) && (difference < (TimeToWaitBeforeSpraying + SprayingTime)*1000))
      {
        //start pumping
        if(!sprayDisabled)
          analogWrite(pumpPin_pwm,pumpSprayStrength);
          
        digitalWrite(pin_led1,HIGH);
      }
      else if((difference > (TimeToWaitBeforeSpraying + SprayingTime)*1000) && (difference < (TimeToWaitBeforeSpraying + SprayingTime + TimeToWaitAfterSpraying)*1000))
      {
        //stop pumping
        analogWrite(pumpPin_pwm,0);
        digitalWrite(pin_led1,LOW);
      }
      //else if((difference > (TimeToWaitBeforeSpraying + SprayingTime + TimeToWaitAfterSpraying)*1000) && (difference < (TimeToWaitBeforeSpraying + SprayingTime + TimeToWaitAfterSpraying + MovingTimeAfterSpray)*1000)) 
      //{
          // move forward for 3 seconds
          //Stop = false;
      //}
      else if (difference > (TimeToWaitBeforeSpraying + SprayingTime + TimeToWaitAfterSpraying + MovingTimeAfterSpray)*1000)
      {
        delayTimeCatch = -1; 
        Spraying = false;
      }
  }
  else if(Spraying == false)
  {
    Spraying = RightSensor(PlantDetectionDistance,RightSensorCheckFrquency);  
  }

  Stop = FrontSensor(FrontSensorObjectDistance,FrontSensorCheckFrquency);

  float potSpace = 180.0f;
  int potVal = analogRead(pin_pot1);
  
  if(potVal<=potSpace)
  {
    movingSpeed = 0.0f;
    Stop = true;
  }
  else 
    movingSpeed=(float)(potVal-potSpace)/(1024.0f-potSpace);

  if(Stop)
  {
      aa = 0;
      bb = 0;
      cc = 0;
      dd = 0;
  }
  else if(Spraying) //crawl the robot
  {  
      aa = movingSpeed*CRAWLING_SPEED_MULTIPLIER;
      bb = movingSpeed*CRAWLING_SPEED_MULTIPLIER;
      cc = movingSpeed*CRAWLING_SPEED_MULTIPLIER;
      dd = movingSpeed*CRAWLING_SPEED_MULTIPLIER;
  }
  else
  {
    aa = movingSpeed;
    bb = movingSpeed;
    cc = movingSpeed;
    dd = movingSpeed;
  }

  //update motor driver signal values 
  driveFR(aa);
  driveFL(bb);
  driveRR(cc);
  driveRL(dd);  
}


//**********FUNCTION DEFINITIONS***************

//return true if an object is detected at right
//else return false;
//objectDetectionDistance -> if there is an object within this distance, return true
//functionFrquency
int counter = 0;
bool FrontSensor(unsigned long objectDetectionDistance, int countFreq)
{
  if(counter < countFreq)
  {
    counter++;
    return false;
  }
  
  digitalWrite(FtrigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(FtrigPin, HIGH);
  
  delayMicroseconds(10); // Added this line
  digitalWrite(FtrigPin, LOW);
  
  unsigned long timeoutDuration = (unsigned long)(58.138*(float)objectDetectionDistance);
  
  unsigned long duration = pulseIn(FechoPin, HIGH,timeoutDuration);
  //unsigned long distance = (duration/58.138);

  if(duration == 0)
    return false;
  else
    return true;
}

//return true if an object is detected at right
//else return false;
//objectDetectionDistance -> if there is an object within this distance, return true
bool RightSensor(unsigned long objectDetectionDistance, float functionFrquency)
{
   unsigned long functionCallPeriod = ( unsigned long)(0.5f + 1000.0f/functionFrquency);
  
  static unsigned long functionTimerBeginTime = millis();
  if((millis() - functionTimerBeginTime) < functionCallPeriod)
  {
    return false;
  }
  functionTimerBeginTime = millis();
  
  digitalWrite(StrigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(StrigPin, HIGH);
  
  delayMicroseconds(10); // Added this line
  digitalWrite(StrigPin, LOW);
  
  unsigned long timeoutDuration = (unsigned long)(0.5f + 58.138f*(float)objectDetectionDistance);
  
  unsigned long duration = pulseIn(SechoPin, HIGH,timeoutDuration);
  //unsigned long distance = (duration/58.138);

  if(duration == 0)
    return false;
  else
    return true;
}

bool drive(double SpeedRPS,long* destFollowAngle,long EncoderVal,double frameDur,int motorDirPin,int motorPwmPin)
{         
  *destFollowAngle = *destFollowAngle + (0.012f * SpeedRPS * frameDur);
  
  long diff = EncoderVal - (long)round(*destFollowAngle);
  double val = (float)maxMOTOR_PWM*2.0f / (1.0f + (float)pow( sigmoid_coefficient,1+ diff)) - (float)maxMOTOR_PWM;

  if(val < 0)
    digitalWrite(motorDirPin,HIGH);
  else
    digitalWrite(motorDirPin,LOW);
  
  analogWrite(motorPwmPin,abs(val));
  if(diff >250)
    return false;
  
  return true;
}





