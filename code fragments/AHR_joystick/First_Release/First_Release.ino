 / *
  * Control of two stepper motors with Arduino and a Joystick
  *
  * Author: Andrea Lombardo
  * Web: http://www.lombardoandrea.com
  * Post: http://wp.me/p27dYH-KQ
  * / 
 // Inclusion of libraries
 #include <AccelStepper.h>
 #include <Bounce2.h>
 // definition of the ants of the Arduino pins
 int ledEnable = 13;  // the led on board will show us the activation status of the motors
  int pinSwEnable = 7;  // the button in the joystick module that enables or disables the control
  int pinEnable = 8;  // the pins that control the ENABLE status of the A4988 drivers are connected in series, so only one pin is needed to manage them both
 unsigned long debounceDelay = 10;  // milliseconds for the button debonuce
  int jX = A0;  // analog pin reading the values ​​for the Xs
  int stepX = 3;  // digital pin that sends the STEP signals to the X driver
  int dirX = 4;  // digital pin that sends the DIRECTION signal to the X driver
 long speedX, valX, mapX;  // variables for managing motor movements X
  int jY = A1;  // analog pin reading the values ​​for Y
  int stepY = 5;  // digital pin that sends the STEP signals to the Y driver
  int dirY = 6;  // digital pin that sends the DIRECTION signal to the Y driver
 long speedY, valY, mapY;  // Y motor movements management variables
 // variables used by the AccelStepper library
  int maxSpeed ​​= 1000;  // according to the library documentation this value can be set up to 4000 for an Arduino UNO
  int minSpeed = 0; // minimum engine speed
  float acceleration = 50.0;  // number of steps per second in acceleration
  int treshold = 30;  // the reading of the potentiometers is never 100% reliable, this value helps to determine the point to consider as "Stay still" in the movements
 long tresholdUp, tresholdDown;  // service variables to perform the task described above
 boolean enabled, moveX, move, enable;  // movements management variables
 Bounce btnEnable = Bounce ();  // instantiate a button from the Bounce library
 // instantiate the engines
 AccelStepper engineX (AccelStepper :: DRIVER, stepX, dirX);
 AccelStepper engineY (AccelStepper :: DRIVER, stepY, dirY);
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
   motoreY.setMaxSpeed ​​(maxSpeed);
   motoreY.setSpeed ​​(minSpeed);
   motoreY.setAcceleration (acceleration);
 }
 void loop () {
   // execute check and read function of the button that determines the enabling status
   checkEnable ();
   digitalWrite (ledEnable, enable);  // show enabling status via LED on pin 13
   digitalWrite (pinEnable,! enable);  // set the opposite value on the ENABLE pins of the drivers
   // perform analogue reading of the values ​​coming from the joystick potentiometers
   valX = analogRead (jX);
   valY = analogRead (jY);
   // map the values ​​read according to the maximum and maximum speed
   mapX = map (valX, 0, 1023, minSpeed, maxSpeed);
   mapY = map (valY, 0, 1023, minSpeed, maxSpeed);
   // run engine command function
   pilotMotors (mapX, mapY);
 }
 void pilotMotors (long mapX, long mapY) {
   if (mapX <= tresholdDown) {
     // x goes back
     speedX = -map (mapX, tresholdDown, minSpeed, minSpeed, maxSpeed);
     movex = true;
   } else if (mapX> = tresholdUp) {
     // x go forward
     speedX = map (mapX, maxSpeed, tresholdUp, maxSpeed, minSpeed);
     movex = true;
   } else {
     // x stands still
     speedX = 0;
     movex = false;
   }
   if (mapY <= tresholdDown) {
     // y goes down
     speedY = -map (mapY, tresholdDown, minSpeed, minSpeed, maxSpeed);
     moveY = true;
   } else if (mapY> = tresholdUp) {
     // y goes up
     speedY = map (mapY, maxSpeed, tresholdUp, maxSpeed, minSpeed);
     moveY = true;
   } else {
     // y stands still
     speedY = 0;
     moveY = false;
   }
   if (muoviX) {
     motoreX.setSpeed ​​(SpeedX);
     motoreX.run ();
   } else {
     motoreX.stop ();
   }
   if (muoviY) {
     motoreY.setSpeed ​​(SpeedY);
     motoreY.run ();
   } else {
     motoreY.stop ();
   }
 }
 void checkEnable () {
   btnEnable.update ();
   if (btnEnable.fell ()) {
     enable =! enable;
   }
 }
