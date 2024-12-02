/*
  Bare Minimum example for driving rotational platform.
  Using TMC2209 driver.
  Thomas Kaufmanas | Random Studio | 2024

*/

#include <TMCStepper.h>
#include <AccelStepper.h>
#include <AS5600.h>
AS5600 as5600;

#define EN_PIN 3    // Enable
#define DIR_PIN 4   // Direction
#define STEP_PIN 5  // Step

#define SW_SCK 6  // Software Slave Clock (SCK)
#define SW_RX 7   // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX 8   // TMC2208/TMC2224 SoftwareSerial transmit pin

#define R_SENSE 0.11f
#define DRIVER_ADDRESS 0b00

SoftwareSerial SoftSerial(SW_RX, SW_TX);
TMC2209Stepper driver(&SoftSerial, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

//Timer logics
long timer1 = 0;
int interval1 = 250;
bool instance = true;

//Rotation and position logics
int stepsPerRevolution = 9600;  //tmc is at 1/8 microsteps 200*8 = 1600 steps pr motor rotation
int stepsPerRevolutionHalf = stepsPerRevolution / 2;
int targetPos = 0;
int realPosWrap = 0;
int stepDiff = 0;
float tolerance = 50;  //tolerance in steps! ≈27 steps pr platform degree

//Serial logics
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

//Encoder logics for absolute position.
//Adjust encZero as needed: +1 moves the point clockwise 1/11th degrees.
int encPos = 0;
int encZero = 344;

void setup() {
  SPI.begin();
  Serial.begin(9600);
  while (!Serial)
    ;
  pinMode(SW_SCK, OUTPUT);
  digitalWrite(SW_SCK, HIGH);
  digitalWrite(DIR_PIN, LOW);

  driver.begin();
  driver.rms_current(1200);
  driver.pwm_autoscale(1);

  Wire.begin();
  as5600.setDirection(AS5600_CLOCK_WISE);

  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(10000);
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();

  resetZero();
}

void loop() {
  //Recieve serial input from TouchDesigner
  recvWithEndMarker();
  showNewNumber();

  //Find wrapped position, ignoring incremental revolutions
  realPosWrap = stepper.currentPosition() % stepsPerRevolution;
  if (realPosWrap < 0) {
    realPosWrap += stepsPerRevolution;
  }

  if (abs(realPosWrap - targetPos) > tolerance) {
    //Find difference, and normalize to locate shortest path
    stepDiff = targetPos - realPosWrap;
    if (stepDiff > stepsPerRevolutionHalf) {
      stepDiff -= stepsPerRevolution;
    } else if (stepDiff < -stepsPerRevolutionHalf) {
      stepDiff += stepsPerRevolution;
    }
    stepper.moveTo(stepper.currentPosition() + stepDiff);
  }

  if (stepper.distanceToGo() != 0) {
    digitalWrite(EN_PIN, LOW);
    stepper.run();
  } else {
    digitalWrite(EN_PIN, HIGH);
  }
}  //end of void

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  if (Serial.available() > 0) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0';  // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewNumber() {
  if (newData == true) {
    float posInput = atof(receivedChars);

    targetPos = mapf(posInput, 10, 370, 0, stepsPerRevolution);
    //Serial.println(posInput);
    newData = false;
  }
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void resetZero() {
  Serial.println("Zeroing platform......");
  encPos = as5600.readAngle();
  while (encPos != encZero) {
    encPos = as5600.readAngle();
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(800);
  }
  Serial.println("Done! Starting program!");
  delay(1000);
}