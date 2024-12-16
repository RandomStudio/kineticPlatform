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



void setup() {
  Serial.begin(9600);

  //SPI.begin();
  //pinMode(SW_SCK, OUTPUT);
  //digitalWrite(SW_SCK, HIGH);
  tmcSerial.begin(115200, SERIAL_8N1, SW_RX, SW_TX);
  driver.begin();
  driver.rms_current(1200);
  driver.pwm_autoscale(true);
  driver.microsteps(8);

  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(3000);
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
}

void loop() {

  if (stepper.distanceToGo() == 0) {
    stepper.move(1600*6);
  }
  stepper.run();
  //Serial.println("yes");
  //Serial.println(targetPos);
}


float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
