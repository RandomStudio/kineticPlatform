/*
  Bare Minimum example for driving rotational platform.
  Using TMC2209 driver.
  Thomas Kaufmanas | Random Studio | 2024

*/

#include <TMCStepper.h>
#include <AccelStepper.h>

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

long timer1 = 0;
int interval1 = 1000 * 1.5;
bool instance = true;

int stepsPerRevolution = 9600;
int targetPosition = 0;

float posInput = 180.0;

const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

void setup() {
  SPI.begin();
  Serial.begin(9600);
  while (!Serial)
    ;
  //Serial.println("Start...");
  pinMode(SW_SCK, OUTPUT);
  digitalWrite(SW_SCK, HIGH);
  driver.begin();            // Initiate pins and registeries
  driver.rms_current(1200);  // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
  //driver.en_pwm_mode(1);      // Enable extremely quiet stepping
  driver.pwm_autoscale(1);
  //driver.microsteps();

  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(10000);
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.disableOutputs();

  //ZERO OUT WITH THE ADS SENSOR HERE
  Serial.println("Please move motor to 0 position! Waiting 5 seconds");
  delay(5000);
  stepper.enableOutputs();
}

void loop() {

  recvWithEndMarker();
  showNewNumber();

  targetPosition = mapf(posInput, 0, 360, 0, stepsPerRevolution);

  if (stepper.currentPosition() != targetPosition) {
    //Serial.print("new position: ");
    //Serial.println(targetPosition);
    stepper.moveTo(targetPosition);
  }
  stepper.run();

  if (millis() - timer1 > interval1) {

    timer1 = millis();
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