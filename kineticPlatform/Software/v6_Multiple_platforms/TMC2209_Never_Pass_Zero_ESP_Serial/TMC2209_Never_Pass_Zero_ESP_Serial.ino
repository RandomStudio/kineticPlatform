#include <WiFi.h>
#include <TMCStepper.h>
#include <AccelStepper.h>

//Stepper variables
#define EN_PIN 5     // Enable
#define DIR_PIN 18   // Direction
#define STEP_PIN 19  // Step

//#define SW_SCK 6  // Software Slave Clock (SCK)
#define SW_RX 16  // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX 17  // TMC2208/TMC2224 SoftwareSerial transmit pin

#define R_SENSE 0.11f
#define DRIVER_ADDRESS 0b00

HardwareSerial tmcSerial(2);
TMC2209Stepper driver(&tmcSerial, R_SENSE, DRIVER_ADDRESS);
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

void setup() {
  Serial.begin(9600);
  tmcSerial.begin(115200, SERIAL_8N1, SW_RX, SW_TX);
  driver.begin();
  driver.rms_current(1200);
  driver.pwm_autoscale(true);
  driver.microsteps(8);

  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(6000);
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
}

void loop() {
  //Recieve serial input from TouchDesigner
  recvWithEndMarker();
  showNewNumber();

  //Find wrapped position, ignoring incremental revolutions
  realPosWrap = stepper.currentPosition();


  if (abs(realPosWrap - targetPos) > tolerance) {
    //Find difference, and normalize to locate shortest path
    stepper.moveTo(targetPos);
  }

  if (stepper.distanceToGo() != 0) {
    digitalWrite(EN_PIN, LOW);
    stepper.run();
  } else {
    digitalWrite(EN_PIN, HIGH);
  }

}  //End of Void

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
