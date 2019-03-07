#include <AccelStepper.h>
#include <Bounce2.h>

const int ledEnable = 13;  //**ON BOARD_PIN **// the led on board will show us the activation status of the motors
const int pinSwEnable = 32;  //**DIGITAL PIN_INPUT APO JOYSTICK **// the button in the joystick module that enables or disables the control
const int pinXDlimit = 3;
const int pinZDlimit = 2;
const int pinYDlimit = 15;
const int pinSWHome = 14;
const int maxDistX = 1080;
const int maxDistY = 710;
//-------------------------------------
const int pinEnableX = 38;  //**DIGITAL  PIN_OUTPUT STO ENABLE TON DRIVER **// the pins that control the ENABLE status of the A4988 drivers are connected in series, so only one pin is needed to manage them both
const int pinEnableY = A2;
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

long speedZ;

int mode; // change value in setup function to change mode

// variables used by the AccelStepper library
const int hspeed = 1500; // homing speed
const int maxSpeed = 2600; // according to the library documentation this value can be set up to 4000 for an Arduino UNO
const int minSpeed = 0;  // minimum engine speed
const int acceleration = 25;  // number of steps per second in acceleration
const int tresholdX = map(maxSpeed, 1000, 4000, 180, 720); // the reading of the potentiometers is never 100% reliable, this value helps to determine the point to consider as "Stay still" in the movements
const int tresholdY = map(maxSpeed, 1000, 4000, 180, 720);
long tresholdUpX, tresholdDownX;  // service variables to perform the task described above
long tresholdUpY, tresholdDownY;
boolean  moveX, moveY, enable;  // movements management variables
Bounce btnEnable = Bounce();  // instantiate a button from the Bounce library
Bounce btnHome = Bounce();


// instantiate the engines

AccelStepper motorX(AccelStepper::DRIVER, stepX, dirX);
AccelStepper motorY(AccelStepper::DRIVER, stepY, dirY);
AccelStepper motorZ(AccelStepper::DRIVER, stepZ, dirZ);

void enableMotors() {
  enable = true;

  digitalWrite(ledEnable, enable);
  digitalWrite(pinEnableX, !enable);   // enable started false
  digitalWrite(pinEnableY, !enable);
  digitalWrite(pinEnableZ, !enable);
}

void disableMotors() {
  enable = false;

  digitalWrite(ledEnable, enable);
  digitalWrite(pinEnableX, !enable);   // enable started false
  digitalWrite(pinEnableY, !enable);
  digitalWrite(pinEnableZ, !enable);
}

void changeMotorState() {
  if (enable)
    disableMotors();
  else
    enableMotors();
}

void setup() {
  // initialize values
  mode = 3; //2 for play , 3 for move to position
  speedX = speedY = speedZ = 0;
  enable = false;
  // definition of the modalities of the pins
  pinMode(ledEnable, OUTPUT);
  pinMode(pinEnableX, OUTPUT);
  pinMode(pinEnableY, OUTPUT);
  pinMode(pinEnableZ, OUTPUT);
  pinMode(pinSwEnable, INPUT_PULLUP);   // the input of the switch needs to be set as INPUT_PULLUP
  pinMode(pinXDlimit, INPUT_PULLUP);
  pinMode(pinYDlimit, INPUT_PULLUP);
  pinMode(pinZDlimit, INPUT_PULLUP);
  pinMode(pinSWHome, INPUT_PULLUP);

  enableMotors();

  // configure the joystick button using the Bounce library
  btnEnable.attach(pinSwEnable);
  btnEnable.interval(debounceDelay);
  // configure the Home button using the Bounce library
  btnHome.attach(pinSWHome);
  btnHome.interval(debounceDelay);
  // calculate range values ​​within which to consider the position of the joystick as "Stand still"
  tresholdDownX = (maxSpeed / 2) - tresholdX;
  tresholdUpX = (maxSpeed / 2) + tresholdX;

  tresholdDownY = (maxSpeed / 2) - tresholdY;
  tresholdUpY = (maxSpeed / 2) + tresholdY;

  //-----------HOMING--------------
  homeProcedure();
  //-------------------------------

  //---------MoveToPosition-------
  if (mode == 3) {
    moveMotorsTo(1070, 1);
    moveMotorsTo(1070, 700);
    moveMotorsTo(1, 700);
    moveMotorsTo(1, 1);
    moveMotorsTo(540, 305);
  }
  
}

void loop() {
  // execute check and read function of the button that determines the enabling status
  checkEnableBtn();
  checkHomeBtn();

  if (mode == 2) {
    valX = analogRead(jX);
    valY = analogRead(jY);

    // map the values ​​read according to the maximum and maximum speed
    mapX = map(valX, 16, 1023, minSpeed, maxSpeed);
    mapY = map(valY, 20, 1023, minSpeed, maxSpeed);

    // run engine command function
    moveMotors(mapX, mapY);
  }
}

void moveMotorsTo(int x, int y) {
  x = -1 * x;
  int posX = 0;
  int dirX = 1;
  int posY = 0;
  int dirY = 1;
  if (x <= 0 && x >= -maxDistX ) {
    delayMicroseconds(200);
    if (motorX.currentPosition() >= x) {
      dirX = -1;
    }
  }
  else {
    motorX.stop();
  }

  if (motorY.currentPosition() >= 0 && motorY.currentPosition() <= maxDistY ) {
    delayMicroseconds(50);
    if (y >= 0 && y <= maxDistY ) {
      delayMicroseconds(200);
      if (motorY.currentPosition() >= y) {
        dirY = -1;
      }
    }
  }
  else {
    motorY.stop();
    motorZ.stop();
  }

  while (motorX.currentPosition() != x || motorY.currentPosition() != y) {
    if (motorX.currentPosition() != x) {
      motorX.moveTo(posX);  // Set the position to move to
      motorX.setSpeed(maxSpeed * dirX);
      motorX.run();  // Start moving the stepper
      posX += dirX; // Increase by 1 for next move if needed
      delayMicroseconds(1000);
    }
    if (motorY.currentPosition() != y) {
      motorY.moveTo(posY);  // Set the position to move to
      motorZ.moveTo(posY);  // Set the position to move to
      motorY.setSpeed(maxSpeed * dirY);
      motorZ.setSpeed(maxSpeed * dirY);
      motorY.run();  // Start moving the stepper
      motorZ.run();  // Start moving the stepper
      posY += dirY; // Increase by 1 for next move if needed
      delayMicroseconds(100);
    }
  }
}

void moveMotors(long mapX, long mapY) {
  if (mapX <= tresholdDownX) { //x reverse
    speedX = -map(mapX, tresholdDownX, minSpeed, minSpeed, maxSpeed);
    moveX = true;
  }
  else if (mapX >= tresholdUpX) { //x forward
    speedX = map(mapX, maxSpeed, tresholdUpX, maxSpeed, minSpeed);
    moveX = true;
  }
  else { //x still
    speedX = 0;
    moveX = false;
  }
  if (mapY <= tresholdDownY) { //y back
    speedY = -map(mapY, tresholdDownY, minSpeed, minSpeed, maxSpeed);
    moveY = true;
  }
  else if (mapY >= tresholdUpY) { //y forward
    speedY = map(mapY, maxSpeed, tresholdUpY, maxSpeed, minSpeed);
    moveY = true;
  }
  else { //y still
    speedY = 0;
    moveY = false;
  }

  if (moveX) {
    if ((motorX.currentPosition() < 0 && motorX.currentPosition() > -maxDistX ) || (motorX.currentPosition () == 0 && speedX < 0) || (motorX.currentPosition() == -maxDistX && speedX > 0) ) {
      motorX.setSpeed(speedX);
      delayMicroseconds(200);
      motorX.run();
    }
  }
  else {
    motorX.stop();
  }

  if (moveY) {
    if ((motorY.currentPosition() > 0 && motorY.currentPosition() < maxDistY ) || (motorY.currentPosition () == 0 && speedY > 0) || (motorY.currentPosition() == maxDistY && speedY < 0)) {
      motorY.setSpeed(speedY);
      motorZ.setSpeed(speedY);
      delayMicroseconds(50);
      motorY.run();
      delayMicroseconds(110);
      motorZ.run();
    }
  }
  else {
    motorY.stop();
    motorZ.stop();
  }
}

void checkEnableBtn() {
  btnEnable.update();
  if (btnEnable.fell()) { //Return 1 when the pin went from HIGH to LOW (fell)
    changeMotorState();
  }
}

void checkHomeBtn() {
  btnHome.update();
  if (btnHome.fell()) {
    if (enable)
      disableMotors();
    homeProcedure();
  }
}

void initMotors(int operation) { //operation 1 for homing, 2 for play, 3 for run to position
  delayMicroseconds(100);
  if (operation == 1) {
    // init motors for homing
    motorX.setMaxSpeed(hspeed);      // Set Max Speed of Stepper (Slower to get better accuracy)
    motorX.setSpeed(hspeed);
    motorX.setAcceleration(0);  // Set Acceleration of Stepper
    // ----------------------------------------------
    motorY.setMaxSpeed(hspeed);      // Set Max Speed of Stepper (Slower to get better accuracy)
    motorY.setSpeed(hspeed);
    motorY.setAcceleration(0);  // Set Acceleration of Stepper
    // ----------------------------------------------
    motorZ.setMaxSpeed(hspeed);      // Set Max Speed of Stepper (Slower to get better accuracy)
    motorZ.setSpeed(hspeed);
    motorZ.setAcceleration(0);  // Set Acceleration of Stepper
  }
  else if (operation == 2) {
    // init motors for play
    motorX.setMaxSpeed(maxSpeed);
    motorX.setSpeed(minSpeed);
    motorX.setAcceleration(acceleration);
    // ----------------------------------------------
    motorY.setMaxSpeed(maxSpeed);
    motorY.setSpeed(minSpeed);
    motorY.setAcceleration(acceleration);
    // ---------------------------------------------
    motorZ.setMaxSpeed(maxSpeed);
    motorZ.setSpeed(minSpeed);
    motorZ.setAcceleration(acceleration);
  }
  else if (operation == 3) {
    // init motors for run to position
    motorX.setMaxSpeed(maxSpeed * 2);
    motorX.setSpeed(minSpeed);
    motorX.setAcceleration(acceleration);
    // ----------------------------------------------
    motorY.setMaxSpeed(maxSpeed * 2);
    motorY.setSpeed(minSpeed);
    motorY.setAcceleration(acceleration);
    // ---------------------------------------------
    motorZ.setMaxSpeed(maxSpeed * 2);
    motorZ.setSpeed(minSpeed);
    motorZ.setAcceleration(acceleration);
  }
  delayMicroseconds(100);
}

void homeProcedure() {
  int hX, hY, hZ;
  delayMicroseconds(100);  // Wait for EasyDriver wake up
  //-------Engine Initialazation-----

  initMotors(1);
  //---------------------------------
  enableMotors();

  //MOTOR X HOMING

  hX = -1;
  while (digitalRead(pinXDlimit)) { // Make the Stepper move CCW until the switch is activatet
    motorX.moveTo(hX);  // Set the position to move to
    motorX.runSpeedToPosition();  // Start moving the stepper
    hX++;  // Increase by 1 for next move if needed
    delayMicroseconds(100);;
  }

  motorX.setCurrentPosition(0);  // Set the current position as zero for now
  hX = 1;

  while (!digitalRead(pinXDlimit)) { // Make the Stepper move CW until the switch is deactivated
    motorX.moveTo(hX);
    motorX.runSpeedToPosition();
    hX--;
    delayMicroseconds(100);
  }

  motorX.setCurrentPosition(0);

  //----------MOTOR Y HOMING -------------------------------
  hY = hZ = -1;
  while (digitalRead(pinYDlimit) || digitalRead(pinZDlimit)) {  // Make the Stepper move CCW until the switch is activatet
    if (digitalRead(pinYDlimit)) {
      motorY.moveTo(hY);  // Set the position to move to
      delayMicroseconds(50);
      motorY.runSpeedToPosition();  // Start moving the stepper
      hY--;  // Decrease by 1 for next move if needed
      delayMicroseconds(100);
    }
    if (digitalRead(pinZDlimit)) {
      motorZ.moveTo(hZ);  // Set the position to move to
      delayMicroseconds(110);
      motorZ.runSpeedToPosition();  // Start moving the stepper
      hZ--;  // Decrease by 1 for next move if needed
      delayMicroseconds(100);
    }
  }

  motorY.setCurrentPosition(0);  // Set the current position as zero for now
  motorZ.setCurrentPosition(0);  // Set the current position as zero for now

  hY = hZ = 1;

  while (!digitalRead(pinYDlimit) || !digitalRead(pinYDlimit)) { // Make the Stepper move CW until the switch is deactivated
    if (!digitalRead(pinYDlimit)) {
      motorY.moveTo(hY);
      delayMicroseconds(50);
      motorY.run();
      hY++;
      delayMicroseconds(100);
    }
    if (!digitalRead(pinZDlimit)) {
      motorZ.moveTo(hZ);
      delayMicroseconds(110);
      motorZ.run();
      hZ++;
      delayMicroseconds(100);
    }
  }

  motorY.setCurrentPosition(0);
  motorZ.setCurrentPosition(0);

  // Homing Completed

  // configure engine parameters
  if (mode == 2) {
    initMotors(3);
    moveMotorsTo(540, 40);
  }

  initMotors(mode);
}
