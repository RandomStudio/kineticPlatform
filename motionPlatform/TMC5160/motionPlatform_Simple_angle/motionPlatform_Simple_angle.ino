/*
  Bare Minimum example for reading angle of rotational platform.
  Based on Rob Tillaarts library demo
  Thomas Kaufmanas
*/

#include "AS5600.h"
AS5600 as5600;

int raw_angle = 0;
int angle = 0;

void setup()
{
  Wire.begin();
  as5600.setDirection(AS5600_CLOCK_WISE);  //  default, just be explicit.

  Serial.begin(115200);
  Serial.println("\nRestart_____\n");
  Serial.print("Connected device: ");
  Serial.print(as5600.isConnected());
  Serial.print("\taddress: ");
  Serial.println(as5600.getAddress());
  delay(4000);
}


void loop()
{
  raw_angle = as5600.readAngle();
  angle = map(raw_angle, 0, 4095, 0, 360);

  Serial.print("Raw angle: ");
  Serial.print(raw_angle);
  Serial.print("\tAngle: ");
  Serial.println(angle);

  delay(100);

} // End of void
