
 #include <AccelStepper.h>
 #include <Bounce2.h>

 const int ledEnable = 13; // the led on board will show us the activation status of the motors
 const int pinSwEnable = 32;  //Joystick input
 
 const int pinEnableX = 38;  //enable pin for drivers
 const int pinEnableY = A2;
 const int pinEnableZ = A8;
 
 unsigned long debounceDelay = 10;  
 
 const int jX = A13;  
 
 const int stepX = A0;  
 const int dirX = A1;  
 
 long speedX, valX, mapX;  
 
 const int jY = A15; 
  
 const int stepY = A6; 
 const int dirY = A7;  
 
 long speedY, valY, mapY; 
 
 const int stepZ = 46; 
 const int dirZ = 48;

 long speedZ, valZ, mapZ;
 

 const int maxSpeed = 1000;  
 const int minSpeed = 0;  
 const float acceleration = 50.0;  
 const int treshold = 175;  
 long tresholdUp, tresholdDown; 
 boolean  muoviX, muoviY,muoviZ ,enable;  
 Bounce btnEnable = Bounce();  

 AccelStepper motorX(AccelStepper::DRIVER, stepX, dirX);
 AccelStepper motorY(AccelStepper::DRIVER, stepY, dirY);
 AccelStepper motorZ(AccelStepper::DRIVER, stepZ, dirZ);
 void setup() {
    Serial.begin(115200);  
   
   speedX = speedY = speedZ = 0;
   enable = false;
   
   pinMode(ledEnable, OUTPUT);
   pinMode(pinEnableX, OUTPUT);
   pinMode(pinEnableY, OUTPUT);
   pinMode(pinEnableZ, OUTPUT);
   
   pinMode(pinSwEnable, INPUT_PULLUP);  
   
   digitalWrite(ledEnable, enable);
   digitalWrite(pinEnableX, !enable); 
   digitalWrite(pinEnableY, !enable);
   digitalWrite(pinEnableZ, !enable);
   
   btnEnable.attach(pinSwEnable);
   btnEnable.interval(debounceDelay);
  
   tresholdDown = (maxSpeed / 2) - treshold;
   tresholdUp = (maxSpeed / 2) + treshold;
 
   motorX.setMaxSpeed(maxSpeed);
   motorX.setSpeed(minSpeed);
   motorX.setAcceleration(acceleration);
   
   motorY.setMaxSpeed(maxSpeed);
   motorY.setSpeed(minSpeed);
   motorY.setAcceleration(acceleration);

   
   motorZ.setMaxSpeed(maxSpeed);
   motorZ.setSpeed(minSpeed);
   motorZ.setAcceleration(acceleration);
 }
 void loop() {
  
   checkEnable();
   digitalWrite(ledEnable, enable);  
   digitalWrite(pinEnableX, !enable);  
   digitalWrite(pinEnableY, !enable);  
   digitalWrite(pinEnableZ, !enable);  
   
   valX = analogRead(jX);
   valY = analogRead(jY);
   //mappa i valori letti in funzione della velocità inima e massima
   mapX = map(valX, 13, 1023, minSpeed, maxSpeed);
   mapY = map(valY, 24, 1023, minSpeed, maxSpeed);
   //mapZ = map(valY, 24, 1023, minSpeed, maxSpeed);

  
 
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
     //x sta fermo
     speedX = 0;
     muoviX = false;
   }
   if (mapY <= tresholdDown) {
     //y va giù
     speedY = -map(mapY, tresholdDown, minSpeed, minSpeed, maxSpeed);
     speedZ = -speedY;
     muoviZ = true;
     muoviY = true;
   } else if (mapY >= tresholdUp) {
     //y va su
     speedY = map(mapY, maxSpeed, tresholdUp, maxSpeed, minSpeed);
     speedZ = -speedY;
     muoviZ = true;
     muoviY = true;
   } else {
     //y sta fermo
     speedY = 0;
     speedZ = 0;
     muoviZ = false;
     muoviY = false;
   }
   if (muoviX) {
     motorX.setSpeed(speedX);
     motorX.runSpeed();
   } else {
     motorX.stop();
   }
   if (muoviY) {
     motorY.setSpeed(speedY);
     motorY.run();
   } else {
     motorY.stop();
   }
   if (muoviZ) {
     motorZ.setSpeed(speedZ);
     motorZ.run();
   } else {
     motorZ.stop();
   }
 }
 void checkEnable() {
   btnEnable.update();
   if (btnEnable.fell()) {
     enable = !enable;
   }
 }
