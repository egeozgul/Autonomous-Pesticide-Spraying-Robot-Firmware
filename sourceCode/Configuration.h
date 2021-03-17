#pragma once
//all pins are here 

//******* Motor setups********//
#define MFR_pwm_Pin  11
#define MFR_dir_Pin  13
#define MFR_encd_A   20
#define MFR_encd_B   24

#define MFL_pwm_Pin  3
#define MFL_dir_Pin  12
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

//******Callibraiton of the motors********//
#define val_Sigmoid 1.0205f

//define to calibrate the motors from analog pin 8 with potensiometer
#define CallibrateMotors

//define to log the calibration value  multiplied by 10000 for precision
//slows down the process too much, avoid using it
//#define LogCallibrationVal

//*******HeadServo***********//
#define headServo_Pin 46

//******ultrasonic Sensor****//
#define pingPin 36

//********pump******
#define pumpPin_pwm 26
#define pumpPin_dir 28

//*******concole pins ******
#define pin_power 36
#define pin_gnd 38

#define pin_led1 44
#define pin_led2 45
#define pin_button1 40
#define pin_button2 42
#define pin_pot1 5
#define pin_pot2 4




