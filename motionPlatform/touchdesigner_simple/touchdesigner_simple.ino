int receivedData;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    receivedData = Serial.read();


    Serial.print("Arduino serial: ");
    Serial.println(receivedData, DEC);

    if (receivedData == 1) {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      digitalWrite(LED_BUILTIN, LOW);
    }

  }

  Serial.println("Hello world");
  delay(500);
}
