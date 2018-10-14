#include <Bounce2.h>
#include <AccelStepper.h>


 // definition of the constants of the Arduino pins
 const int ledEnable = 13;  //**ON BOARD_PIN **// the led on board will show us the activation status of the motors
 const int pinSwEnable = 7; //**DIGITAL PIN_INPUT APO JOYSTICK **// the button in the joystick module that enables or disables the control
 
 const int pinEnableX = 38;  //**DIGITAL  PIN_OUTPUT STO ENABLE TON DRIVER **// the pins that control the ENABLE status of the A4988 drivers are connected in series, so only one pin is needed to manage them both
 const int pinEnableY = A2;
 const int pinEnableZ = A8;
 
 unsigned long debounceDelay = 10;  // milliseconds for the button debonuce
 
 const int jX = A13;  //**ANALOG INPUT- APO JOYSTICK **// analog pin reading the values ​​for the Xs
 const int stepX = A0;  // ** DIGITAL PIN OUT PWM STEP X STO DRIVER **// digital pin that sends the STEP signals to the X driver
 const int dirX = A1;  // ** DIGITAL PIN OUT STO DRIVER **// digital pin that sends the DIRECTION signal to the X driver
 
 long speedX, valX, mapX;  // variables for managing motor movements X
 
 const int jY = A15;  //**ANALOG INPUT- APO JOYSTICK **// // analog pin reading the values ​​for Y
 const int stepY = A6;  // ** DIGITAL PIN OUT PWM STEP X STO DRIVER **// // digital pin that sends the STEP signals to the Y driver
 const int dirY = A7;  // ** DIGITAL PIN OUT STO DRIVER **// // digital pin that sends the DIRECTION signal to the Y driver
 
 long speedY, valY, mapY;  // Y motor movements management variables
 
 const int stepZ = 46;  // ** DIGITAL PIN OUT PWM STEP X STO DRIVER **// digital pin that sends the STEP signals to the X driver
 const int dirZ = 48;
 
 long speedZ, valZ, mapZ;
 
 // variables used by the AccelStepper library
 const int maxSpeed ​​= 1000;  // according to the library documentation this value can be set up to 4000 for an Arduino UNO
 const int minSpeed ​​= 0;  // minimum engine speed
 const float acceleration = 50.0;  // number of steps per second in acceleration
 const int treshold = 30;  // the reading of the potentiometers is never 100% reliable, this value helps to determine the point to consider as "Stay still" in the movements
 
 long tresholdUp, tresholdDown;  // service variables to perform the task described above
 boolean  abilitato,moveX, moveY, enable;  // movements management variables
 Bounce btnEnable = Bounce ();  // instantiate a button from the Bounce library
 
 // instantiate the engines
 AccelStepper motoreX (AccelStepper :: DRIVER, stepX, dirX); // with step pin & dir pin
 AccelStepper motoreY (AccelStepper :: DRIVER, stepY, dirY);
 AccelStepper motoreY (AccelStepper :: DRIVER, stepZ, dirZ);
 
 void setup () {
   // initialize values
   speedX = speedY = 0;
   enable = false;
   
   // definition of the modalities of the pins
   pinMode (ledEnable, OUTPUT);
   pinMode (pinEnable, OUTPUT);
   pinMode (pinSwEnable, INPUT_PULLUP);  // the input of the switch needs to be set as INPUT_PULLUP
   
   digitalWrite (ledEnable, enable);
   digitalWrite (pinEnable,! enable);  // The A4988 drivers disable the motor commands if on the ENABLE pin they receive a HIGH signal for this reason the value is opposite to that of the LED
   // configure the joystick button using the Bounce library
   
   btnEnable.attach (pinSwEnable);
   btnEnable.interval (debounceDelay);
   // calculate range values ​​within which to consider the position of the joystick as "Stand still"
   
   tresholdDown = (maxSpeed ​​/ 2) - treshold;
   tresholdUp = (maxSpeed ​​/ 2) + treshold;
   
   // configure engine parameters
   motoreX.setMaxSpeed ​​(maxSpeed);
   motoreX.setSpeed ​​(minSpeed);
   motoreX.setAcceleration (acceleration);
   
   motoreY.setMaxSpeed ​​(maxiSpeed);
   motoreY.setSpeed ​​(minSpeed);
   motoreY.setAcceleration (acceleration);
   
   motoreZ.setMaxSpeed ​​(maxiSpeed);
   motoreZ.setSpeed ​​(minSpeed);
   motoreZ.setAcceleration (acceleration);
 }
 
 void loop () {
   // execute check and read function of the button that determines the enabling status
   checkEnable ();
   digitalWrite (ledEnable, enable);  // show enabling status via LED on pin 13
   digitalWrite (pinEnable,! enable);  // set the opposite value on the ENABLE pins of the drivers
   
   // perform analogue reading of the values ​​coming from the joystick potentiometers
   valX = analogRead (jX);
   valY = analogRead (jY);
  // valZ = analogRead (jY);
   
   // map the values ​​read according to the maximum and maximum speed
   mapX = map (valX, 0, 1023, minSpeed, maxSpeed);
   mapY = map (valY, 0, 1023, minSpeed, maxSpeed);
  // mapZ = map (valZ, 0, 1023, minSpeed, maxSpeed);
   
   // run engine command function
   pilotMotors (mapX, mapY);
 }
 void pilotMotors (long mapX, long mapY) {
   
   if (mapX <= tresholdDown) {
     // x goes back
     speedX = -map (mapX, tresholdDown, minSpeed, minSpeed, maxSpeed);
     moveX = true;
   } else if (mapX> = tresholdUp) {
     // x go forward
     speedX = map (mapX, maxSpeed, tresholdUp, maxSpeed, minSpeed);
     moveX = true;
   } else {
     // x stands still
     speedX = 0;
     moveX = false;
   }
   
   if (mapY <= tresholdDown) {
     // y goes down
     speedY = -map (mapY, tresholdDown, minSpeed, minSpeed, maxSpeed);
     speedZ = -map (mapY, tresholdDown, minSpeed, minSpeed, maxSpeed);
     moveY = true;
   } else if (mapY> = tresholdUp) {
     // y goes up
     speedY = map (mapY, maxiSpeed, tresholdUp, maxSpeed, minSpeed);
     speedZ = map (mapY, maxiSpeed, tresholdUp, maxSpeed, minSpeed);
     moveY = true;
   } else {
     // y stands still
     speedY = 0;
     speedZ = 0;
     moveY = false;
     moveZ = false;
   }
   
   if (moveX) {
     motoreX.setSpeed ​​(SpeedX);
     motoreX.run ();
   } else {
     motoreX.stop ();
   }
   
   if (moveY) {
     motoreY.setSpeed ​​(SpeedY);
     motoreZ.setSpeed ​​(SpeedY);
     motoreY.run ();
   motoreZ.run ();
   } else {
     motoreY.stop ();
     motoreZ.stop ();
   }
 }
 void checkEnable () {
   btnEnable.update ();
   if (btnEnable.fell ()) {
     enable =! enable;
   }
 }
