/*
  Bare Minimum example for driving rotational platform.
  Based on Watterott example (CC0 licence)
  Thomas Kaufmanas | Random Studio | 2024

*/

#include <TMCStepper.h>

#define EN_PIN    7 //enable
#define DIR_PIN   8 //direction
#define STEP_PIN  9 //step
#define CS_PIN   10 //CS
#define MOSI_PIN 11 //SDI/MOSI
#define MISO_PIN 12 //SDO/MISO
#define SCK_PIN  13 //CLK/SCK  
#define R_SENSE   0.075f //Respective to QHV5160 TMC5160 module

TMC5160Stepper tmc = TMC5160Stepper(CS_PIN, R_SENSE);

void setup()
{
  pinMode(EN_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH); //deactivate driver for now...
  digitalWrite(DIR_PIN, LOW);
  digitalWrite(STEP_PIN, LOW);

  //SPI setup on hardware pins: 10, 11, 12, 13
  pinMode(SS, OUTPUT);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT);
  pinMode(SCK, OUTPUT);
  digitalWrite(SS, HIGH);
  digitalWrite(MOSI, LOW);
  digitalWrite(MISO, HIGH);
  digitalWrite(SCK, LOW);

  Serial.begin(9600); //init serial port and set baudrate
  Serial.println("\nRestart ______________");

  tmc.begin();
  tmc.toff(4); // This might be only for TMC2208
  tmc.blank_time(24); //And this as well?
  tmc.en_pwm_mode(true); //Enable StealthChop (quiet driving)
  tmc.pwm_autoscale(true); //Some say this is needed?
  tmc.microsteps(1);  //NB - adding 16 microstep resolution breaks?
  tmc.rms_current(2000); //Motor runs 2.8A peak ≈ keep it round 2 for good measure.

  digitalWrite(EN_PIN, LOW); //Enable motor
}

void loop()
{
  static uint32_t last_time = 0;
  uint32_t ms = millis();

  if ((ms - last_time) > 1000) //DIAGNOSTICS run
  {
    last_time = ms;

    if (tmc.diag0_error()) {
      Serial.println(F("DIAG0 error"));
    }
    if (tmc.ot())         {
      Serial.println(F("Overtemp."));
    }
    if (tmc.otpw())       {
      Serial.println(F("Overtemp. PW"));
    }
    if (tmc.s2ga())       {
      Serial.println(F("Short to Gnd A"));
    }
    if (tmc.s2gb())       {
      Serial.println(F("Short to Gnd B"));
    }
    if (tmc.ola())        {
      Serial.println(F("Open Load A"));
    }
    if (tmc.olb())        {
      Serial.println(F("Open Load B"));
    }
  }

  //make steps
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(10);
}
