/*
  Bare Minimum example for driving rotational platform.
  Using TMC2208 driver.
  Thomas Kaufmanas | Random Studio | 2024

*/
#include <TMCStepper.h>
#include <SoftwareSerial.h>
#include <AS5600.h>
AS5600 as5600;

#define DIR_PIN_1 3   // Direction
#define STEP_PIN_1 4  // Step
#define EN_PIN_1 5    // Enable

#define SW_RX 12
#define SW_TX 10
#define DRIVER_ADDRESS 0b00
#define R_SENSE 0.11f  // TMC2208
//TMC2209Stepper driver(SW_RX, SW_TX, R_SENSE, DRIVER_ADDRESS);
TMC2208Stepper driver(SW_RX, SW_TX, R_SENSE);

int accSpeed = 1000;
int maxSpeed = 200;
int minSpeed = 1000;


long timer1 = 0;
int interval1 = 500;
int accDir = 0;  //0 = down, 1 = up
float raw_angle = 0;
float angle = 0;
float posInput = 180;  //180 is default
float angleDiff = 0;
int dir = false;
float tolerance = 1;

const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

#include <AccelStepper.h>
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN_1, DIR_PIN_1);

void setup() {
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, LOW);

  Wire.begin();
  as5600.setDirection(AS5600_CLOCK_WISE);  //  default, just be explicit.

  Serial.begin(9600);
  Serial.setTimeout(200);

  driver.beginSerial(115200);
  driver.begin();
  driver.toff(5);
  driver.rms_current(1200);  // Set motor RMS current
  //driver.en_pwm_mode(true); //Enable StealthChop (quiet driving)
  driver.pwm_autoscale(true);  // Needed for stealthChop

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(1000);
  stepper.setEnablePin(EN_PIN_1);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
}

void loop() {

  recvWithEndMarker();
  showNewNumber();

  raw_angle = as5600.readAngle();
  angle = mapf(raw_angle, 0, 4095, 0, 360);
  //Serial.println(angle);

  if (abs(angle - posInput) > tolerance) {
    //Find difference, and normalize to locate shortest path
    angleDiff = posInput - angle;
    if (angleDiff > 180) {
      angleDiff -= 360;
    } else if (angleDiff < -180) {
      angleDiff += 360;
    }

    // Adjust speed relative to traveldistance
    int a = abs(angleDiff);
    //Serial.println(a); //For debug only - drastically decreases speeds.
    if (a < 30) {
      accSpeed = map(a, 0, 30, minSpeed, maxSpeed);
    } else {
      accSpeed = maxSpeed;
    }




    //CHOSE DIRECTION
    if (angleDiff > 0) {
      if (!dir) {
        dir = true;
        digitalWrite(DIR_PIN_1, HIGH);
      }
    } else if (angleDiff < 0) {
      if (dir) {
        dir = false;
        digitalWrite(DIR_PIN_1, LOW);
      }
    }

    digitalWrite(EN_PIN_1, LOW);

    digitalWrite(STEP_PIN_1, HIGH);
    delayMicroseconds(10);
    digitalWrite(STEP_PIN_1, LOW);
    delayMicroseconds(accSpeed);
  } else {
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
    posInput = atof(receivedChars);
    //Serial.println(posInput);
    newData = false;
  }
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
