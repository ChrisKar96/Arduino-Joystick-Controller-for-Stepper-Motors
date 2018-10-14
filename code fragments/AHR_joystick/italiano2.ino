
 #include <AccelStepper.h>
 #include <Bounce2.h>

 const int ledEnable = 13;  //**ON BOARD_PIN **// the led on board will show us the activation status of the motors
 const int pinSwEnable = 32;  //**DIGITAL PIN_INPUT APO JOYSTICK **// the button in the joystick module that enables or disables the control
 const int pinEnableX = 38;  //**DIGITAL  PIN_OUTPUT STO ENABLE TON DRIVER **// the pins that control the ENABLE status of the A4988 drivers are connected in series, so only one pin is needed to manage them both
 const int pinEnableZ = A8;
 unsigned long debounceDelay = 10;  // milliseconds for the button debonuce
 
 const int jX = A13;  //**ANALOG INPUT- APO JOYSTICK **// analog pin reading the values ​​for the Xs
 
 const int stepX = A0;  // ** DIGITAL PIN OUT PWM STEP X STO DRIVER **// digital pin that sends the STEP signals to the X driver 
 const int dirX = A1;  // ** DIGITAL PIN OUT STO DRIVER **// digital pin that sends the DIRECTION signal to the X driver
 
 long speedX, valX, mapX;  // variables for managing motor movements X
 
 const int jY = A15;   //**ANALOG INPUT- APO JOYSTICK **// // analog pin reading the values ​​for Y
 const int stepY = A6;  // ** DIGITAL PIN OUT PWM STEP X STO DRIVER **// // digital pin that sends the STEP signals to the Y driver
 const int dirY = A7; // ** DIGITAL PIN OUT STO DRIVER **// // digital pin that sends the DIRECTION signal to the Y driver
 
 long speedY, valY, mapY;  // Y motor movements management variables

 const int stepZ = 46;  // ** DIGITAL PIN OUT PWM STEP X STO DRIVER **// digital pin that sends the STEP signals to the X driver
 const int dirZ = 48;

 long speedZ, valZ, mapZ;
 
// variables used by the AccelStepper library
 const int maxSpeed = 1000; // according to the library documentation this value can be set up to 4000 for an Arduino UNO
 const int minSpeed = 0;  // minimum engine speed
 const float accelerazione = 20.0;  // number of steps per second in acceleration
 const int treshold = 175;  // the reading of the potentiometers is never 100% reliable, this value helps to determine the point to consider as "Stay still" in the movements
 long tresholdUp, tresholdDown;  // service variables to perform the task described above
 boolean abilitato, muoviX, muoviY, enable;  // movements management variables
 Bounce btnEnable = Bounce();  // instantiate a button from the Bounce library
 
 
 // instantiate the engines

 AccelStepper motoreX(AccelStepper::DRIVER, stepX, dirX);
 AccelStepper motoreY(AccelStepper::DRIVER, stepY, dirY);
 AccelStepper motoreZ(AccelStepper::DRIVER, stepY, dirY);
 void setup() {
   // initialize values

   speedX = speedY = speedY = 0;
   enable = false;
   
// definition of the modalities of the pins
   pinMode(ledEnable, OUTPUT);
   pinMode(pinEnableX, OUTPUT);
   pinMode(pinEnableY, OUTPUT);
   pinMode(pinEnableZ, OUTPUT);
   pinMode(pinSwEnable, INPUT_PULLUP);   // the input of the switch needs to be set as INPUT_PULLUP
   digitalWrite(ledEnable, enable);
   digitalWrite(pinEnableX, !enable);   // The A4988 drivers disable the motor commands if on the ENABLE pin they receive a HIGH signal for this reason the value is opposite to that of the LED
   digitalWrite(pinEnableY, !enable);
   digitalWrite(pinEnableZ, !enable);
   // configure the joystick button using the Bounce library
   btnEnable.attach(pinSwEnable);
   btnEnable.interval(debounceDelay);
   // calculate range values ​​within which to consider the position of the joystick as "Stand still"
   tresholdDown = (maxSpeed / 2) - treshold;
   tresholdUp = (maxSpeed / 2) + treshold;
   // configure engine parameters
   motoreX.setMaxSpeed(maxSpeed);
   motoreX.setSpeed(minSpeed);
   motoreX.setAcceleration(accelerazione);
   motoreY.setMaxSpeed(maxSpeed);
   motoreY.setSpeed(minSpeed);
   motoreY.setAcceleration(accelerazione);
 }
 void loop() {
   // execute check and read function of the button that determines the enabling status
   checkEnable();
   digitalWrite(ledEnable, enable);  // show enabling status via LED on pin 13
   digitalWrite(pinEnableX, !enable);  
   digitalWrite(pinEnableY, !enable); 
   digitalWrite(pinEnableZ, !enable); 
   
  // perform analogue reading of the values ​​coming from the joystick potentiometers
   valX = analogRead(jX);
   valY = analogRead(jY);
   // map the values ​​read according to the maximum and maximum speed
   mapX = map(valX, 13, 1023, minSpeed, maxSpeed);
   mapY = map(valY, 24, 1023, minSpeed, maxSpeed);


   // run engine command function
   pilotaMotori(mapX, mapY);
 }
 void pilotaMotori(long mapX, long mapY) {
  
   if (mapX <= tresholdDown) {
     //x back
     speedX = -map(mapX, tresholdDown, minSpeed, minSpeed, maxSpeed);
     muoviX = true;
   } else if (mapX >= tresholdUp) {
     //x forward
     speedX = map(mapX, maxSpeed, tresholdUp, maxSpeed, minSpeed);
     muoviX = true;
   } else {
     //x still
     speedX = 0;
     muoviX = false;
   }
   if (mapY <= tresholdDown) {
     //y va giù
     speedY = -map(mapY, tresholdDown, minSpeed, minSpeed, maxSpeed);
     muoviY = true;
   } else if (mapY >= tresholdUp) {
     //y va su
     speedY = map(mapY, maxSpeed, tresholdUp, maxSpeed, minSpeed);
     muoviY = true;
   } else {
     //y down		
     speedY = 0;
     muoviY = false;
   }
   if (muoviX) {
     motoreX.setSpeed(speedX);
     motoreX.run();
   } else {
     motoreX.stop();
   }
   if (muoviY) {
     motoreY.setSpeed(speedY);
     motoreY.run();
   } else {
     motoreY.stop();
   }
 }
 void checkEnable() {
   btnEnable.update();
   if (btnEnable.fell()) {
     enable = !enable;
   }
 }
