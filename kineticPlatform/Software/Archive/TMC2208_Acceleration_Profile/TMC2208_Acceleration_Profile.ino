/*
  Bare Minimum example for accelerating rotation platform using TMC2208 driver.
  Thomas Kaufmanas | Random Studio | 2024

*/
#include <TMCStepper.h>
#include <SoftwareSerial.h>  // Software serial for the UART to TMC2209 - https://www.arduino.cc/en/Reference/softwareSerial

// TMC2208 DRIVER

#define DIR_PIN_1 3   // Direction
#define STEP_PIN_1 4  // Step
#define EN_PIN_1 5    // Enable


#define SW_RX 12             // TMC2208/TMC2224 SoftwareSerial receive pins
#define SW_TX 10             // TMC2208/TMC2224 SoftwareSerial transmit pin
#define DRIVER_ADDRESS 0b00  // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f        // Match to your driver
#define MICROSTEPS 1
#define STEPS_PER_REVOLUTION 200  //this makes it have 16 more steps = 3200 steps in a full spin
TMC2209Stepper driver(SW_RX, SW_TX, R_SENSE, DRIVER_ADDRESS);

int speed1 = 1000;
long timer1 = 0;
long timer3 = 0;
int interval3 = 2000;
int interval1 = 5;
int accDir = 0;  //0 = down, 1 = up

void setup() {
  pinMode(EN_PIN_1, OUTPUT);
  pinMode(STEP_PIN_1, OUTPUT);
  pinMode(DIR_PIN_1, OUTPUT);
  digitalWrite(EN_PIN_1, LOW);

  Serial.begin(9600);

  driver.beginSerial(115200);
  driver.begin();
  driver.toff(5);
  driver.rms_current(1200);  // Set motor RMS current
  //driver.en_pwm_mode(true); //Enable StealthChop (quiet driving)
  driver.pwm_autoscale(true);  // Needed for stealthChop
}

void loop() {


  if (millis() - timer1 > interval1) {

    if (!accDir) {
      speed1 -= 5;
    } else {
      speed1 += 5;
    }

    if (speed1 > 1000) {
      accDir = 0;
    }
    if (speed1 < 200) {
      for (int i = 0; i <= 10000; i++) {
        digitalWrite(STEP_PIN_1, HIGH);
        delayMicroseconds(10);
        digitalWrite(STEP_PIN_1, LOW);
        delayMicroseconds(speed1);
      }
      accDir = 1;
    }

    //Serial.println(speed1);

    timer1 = millis();
  }

  digitalWrite(STEP_PIN_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(STEP_PIN_1, LOW);
  delayMicroseconds(speed1);
}
