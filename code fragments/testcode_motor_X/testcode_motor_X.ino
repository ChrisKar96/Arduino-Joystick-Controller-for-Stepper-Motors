 #include <AccelStepper.h>
 #include <Bounce2.h>

 const int pinEnableX = 38;  //enable pin for drivers
 const int jX = A13;  
 const int stepX = A0;  
 const int dirX = A1;  
 
 long speedX, valX, mapX;

 const int maxSpeed = 4000;  
 const int minSpeed = 0;  
 const int acceleration = 80000; 
 const int treshold = 175;  
 
 long tresholdUp, tresholdDown; 
 boolean  muoviX,enable;  
 Bounce btnEnable = Bounce();  

 AccelStepper motorX(AccelStepper::DRIVER, stepX, dirX);

void setup() {
  Serial.print(9600);
speedX =  0;
pinMode(pinEnableX, OUTPUT);


  tresholdDown = (maxSpeed / 2) - treshold;
   tresholdUp = (maxSpeed / 2) + treshold;
 
   motorX.setMaxSpeed(maxSpeed);
   motorX.setSpeed(maxSpeed);
   motorX.setAcceleration(acceleration);
}

void loop() {
   valX = analogRead(jX);
 mapX = map(valX, 13, 1023, minSpeed, maxSpeed);
  pilotaMotori(mapX);
  Serial.print(mapX);
 }
 void pilotaMotori(long mapX) {
   if (mapX <= tresholdDown) {
     //x back
     speedX = -map(mapX, tresholdDown, minSpeed, minSpeed, maxSpeed);
     muoviX = true;
   } else if (mapX >= tresholdUp) {
     //x forward
     speedX = map(mapX, maxSpeed, tresholdUp, maxSpeed, minSpeed);
     muoviX = true;
   } else {
     speedX = 0;
     muoviX = false;
   }
    if (muoviX) {
     motorX.setSpeed(speedX);
     motorX.run();
   } else {
     motorX.stop();
   }
}
