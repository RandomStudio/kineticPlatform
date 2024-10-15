int receivedData;
char buffer[5];   //maximum expected length
int len = 0;
int vall = 0;

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
