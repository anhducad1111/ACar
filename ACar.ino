#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Servo.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

FirebaseData fbdo;
String path = "/";
String uid;
FirebaseJson json;
FirebaseAuth auth;
FirebaseConfig config;
Servo servo;

int i;
int x = 0;
long duration;
float distance, distanceLeft, distanceRight ;
bool dataWritten = false; 
int count = 0; 
int address = 0; 
bool resetFlag = false;
int maxspeed = 100;

#define TRIG 13
#define ECHO 15
#define SERV 16
#define ENA 5
#define IN1 4
#define IN2 0
#define ENB 12
#define IN3 2
#define IN4 14
#define WIFI_SSID "Duc"
#define WIFI_PASSWORD "123456789"
#define API_KEY "AIzaSyAk_KLxh6MjPmra7t1n2VqkYufjQ4y3ABE"
#define FIREBASE_PROJECT_ID "acar-9d140"
#define DATABASE_URL "acar-9d140-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "5lLOKpU81EPoTKswxaCZMXZDExwGL0lgiTcel7Im"
#define SOUND_VELOCITY 0.034
#define USER_EMAIL "anhduc@gmail.com"
#define USER_PASSWORD "111111"


void setup() {
  servo.attach(SERV);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 0);
  digitalWrite(ENA, 0);
  digitalWrite(ENB, 0);

  Serial.begin(9600);

  //config wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(".");
  }
  Serial.println("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  //config firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  config.service_account.data.project_id = FIREBASE_PROJECT_ID;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
  Firebase.reconnectWiFi(true);
  if (!Firebase.beginStream(fbdo, path))
  {
    Serial.println("REASON: "+ fbdo.errorReason());
  }

  //EEPROM
  EEPROM.begin(512);
  count = EEPROM.read(address);
  if (count > 0) {
    count = 0; 
  }
  EEPROM.end();

}

void measureDistance(){
  digitalWrite(TRIG, 0);
  delayMicroseconds(2);
  
  digitalWrite(TRIG, 1);
  delayMicroseconds(10);
  digitalWrite(TRIG, 0);

  duration = pulseIn(ECHO, 1);

  distance = duration * SOUND_VELOCITY / 2;

  Serial.print("Distance (cm): ");
  Serial.println(distance);
  delay(500);
}

void goForward(int speed1, int speed2){
  analogWrite(ENA, speed1);
  analogWrite(ENB, speed2);
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 1);
  Serial.println("Going ahead");
}

void turnCarRight(){
  analogWrite(ENA, 150);
  analogWrite(ENB, 0);
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 1);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 0);
  Serial.println("Going Right");
}

void turnCarLeft(){
  analogWrite(ENA, 0);
  analogWrite(ENB, 100);
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 1);
  Serial.println("Going Left");
}

void goBackward(){
  analogWrite(ENA, 75);
  analogWrite(ENB, 50);
  digitalWrite(IN1, 1);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 1);
  digitalWrite(IN4, 0);
  Serial.println("Going back");
}

void stop(){
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, 0);
  digitalWrite(IN2, 0);
  digitalWrite(IN3, 0);
  digitalWrite(IN4, 0);
  Serial.println("Stop");
}

void turnSensorLeft(){
  servo.write(210); 
  delay(1000);
}

void turnSensorRight(){
  servo.write(180);
  delay(1000);
}

void resetServo(){
  servo.write(90);
}

void determineDirection() {
  stop();
  delay(1000);
  goBackward();
  delay(1000);
  stop();
  delay(1000);
  turnSensorLeft();
  measureDistance();
  distanceLeft = distance;
  delay(500);
  resetServo();
  turnSensorRight();
  measureDistance();
  distanceRight = distance;
  delay(500);
  resetServo();
  if (distanceLeft <= 15 && distanceRight <= 15) {
    handleBothSidesClose();
  } else if (distanceLeft <= distanceRight) {
    handleLeftClose();
  } else if (distanceLeft > distanceRight){
    handleRightClose();
  }
}

void handleBothSidesClose() {
  stop();
  delay(500);
  goBackward();
  delay(500);
  stop();
  delay(1000);
  turnSensorLeft();
  measureDistance();
  distanceLeft = distance;
  delay(500);
  resetServo();
  turnSensorRight();
  measureDistance();
  distanceRight = distance;
  delay(300);
  resetServo();
}

void handleLeftClose() {
  delay(300);
  turnCarRight();
  delay(300);
}

void handleRightClose() {
  delay(300);
  turnCarLeft();
  delay(300);
}

void pushDataToDatabase(int i, String s){
  String dataPath = s+"/data"; 
  String dataPayload = String(i); 
  Firebase.setString(fbdo, dataPath, dataPayload);
  if (fbdo.errorReason() == NULL) {
    Serial.println("Đẩy dữ liệu lên Firebase thành công!");
  } else {
    Serial.println("Đẩy dữ liệu lên Firebase thất bại!");
    Serial.println(fbdo.errorReason());
  }
  dataWritten = true; 
  EEPROM.begin(512);
  EEPROM.write(address, count); 
  EEPROM.commit();
  EEPROM.end();   
}

void loop() {
  measureDistance();
  if (distance > 40) {
    goForward(150,100);
  } else if (distance <= 40 && distance > 15) {
    goForward(75,50);
  } else if (distance <= 15) {
    determineDirection();
    count++;
  }
}
