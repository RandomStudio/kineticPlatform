#include <WiFi.h>
#include <ArduinoMqttClient.h>
#include <ArduinoJson.h>  // ArduinoJson v6 has built-in MessagePack (de)serialization capabilities
//ArduinoJson.H version 6.15.0 seems to compile fine, but newer versions does not.

#include <TMCStepper.h>
#include <AccelStepper.h>

// Wifi + MQTT setup variables
#define WLAN_SSID "tether_2.4"
#define WLAN_PASS "sp_ceB0ss!"
#define MQTT_IP "10.0.0.105"
#define MQTT_PORT 1883
#define MQTT_USER "tether"
#define MQTT_PASS "sp_ceB0ss!"
#define ID "platform1"
String inputTopic = "";
String outputTopic = "tetherduino/output/mcuData";

WiFiClient wifiClient;
MqttClient mqtt(wifiClient);

StaticJsonDocument<512> inputDoc;
StaticJsonDocument<16> outputDoc;
std::string outputMessage;


//Stepper variables
#define EN_PIN 5     // Enable
#define DIR_PIN 18   // Direction
#define STEP_PIN 19  // Step

#define SW_RX 16  // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX 17  // TMC2208/TMC2224 SoftwareSerial transmit pin

#define R_SENSE 0.11f
#define DRIVER_ADDRESS 0b00

HardwareSerial tmcSerial(2);
TMC2209Stepper driver(&tmcSerial, R_SENSE, DRIVER_ADDRESS);
AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

//Timer logics
long timer1 = 0;
int interval1 = 1000;
bool instance = true;

//Rotation and position logics
int stepsPerRevolution = 9600;  //tmc is at 1/8 microsteps 200*8 = 1600 steps pr motor rotation
int stepsPerRevolutionHalf = stepsPerRevolution / 2;
int targetPos = 0;
int realPosWrap = 0;
int stepDiff = 0;
float tolerance = 50;  //tolerance in steps! ≈27 steps pr platform degree

const byte inputLen = 8;
uint8_t buf[inputLen];


void setup() {
  Serial.begin(9600);
  inputTopic = "td/" + String(ID) + "/angles";
  delay(10);
  connectWiFi();
  mqtt.onMessage(onMqttMessage);

  tmcSerial.begin(115200, SERIAL_8N1, SW_RX, SW_TX);
  driver.begin();
  driver.rms_current(2000);
  driver.pwm_autoscale(true);
  driver.microsteps(8);

  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(3000);
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
}

void loop() {
  stepper.run();
  // Keep mqtt connection alive. On interval to retain performance of accelleration ramp
  if (millis() - timer1 > interval1) {
    MQTT_connect();
    mqtt.poll();
    timer1 = millis();
  }

  if (!mqtt.connected()) {
    mqtt.stop();
  }

  if (stepper.distanceToGo() == 0) {
    stepper.move(9600);
  }


  //Serial.println(targetPos);
}





void connectWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(2000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}




void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected()) {
    return;
  }
  Serial.println("Connecting to MQTT... ");
  mqtt.unsubscribe(inputTopic);
  mqtt.setUsernamePassword(MQTT_USER, MQTT_PASS);
  while (!mqtt.connect(MQTT_IP, MQTT_PORT)) {
    Serial.println(mqtt.connectError());
    Serial.println("Retrying MQTT connection in 5 seconds...");
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
  mqtt.subscribe(inputTopic);
}

void send(int number) {
  outputDoc["number"] = number;                // set the data
  outputMessage = "";                          // clear the output string
  serializeMsgPack(outputDoc, outputMessage);  // serialize the data
  // send
  mqtt.beginMessage(outputTopic);
  // mqtt.print(outputMessage.c_str());
  for (int i = 0; i < outputMessage.size(); i++) {
    mqtt.print(outputMessage[i]);
  }
  mqtt.endMessage();
  Serial.println("Published number: " + String(number));
}


void onMqttMessage(int len) {
  int i = 0;
  while (mqtt.available()) {
    buf[i] = mqtt.read();
    ++i;
  }
  
  deserializeMsgPack(inputDoc, buf);
  float value = inputDoc.as<float>();
  //Serial.print("received angle: ");
  //Serial.println(String(value));
  targetPos = mapf(value, 10, 370, 0, stepsPerRevolution);
  Serial.println(targetPos);

  //decodeMessage(buf, len);
  //send(inputDoc["someNumber"].as<int>());


  
}


float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
