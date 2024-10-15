int receivedData;

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {


    receivedData = Serial.parseInt();

    if (receivedData >= 10) {
      Serial.println(receivedData);
    }


    //Serial.println(receivedData);
    //Serial.print("\n");
  }


  //Serial.println("hw");

}
