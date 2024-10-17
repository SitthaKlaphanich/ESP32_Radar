#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "html.h"

const char* ssid = "----";
const char* password = "----";

#define LED 2
#define TrigPin 5
#define EchoPin 18
#define BUZZER_PIN 4  // กำหนดพินให้กับ buzzer
#define SoundSpeed 0.034

float dangerDistance  ; // ระยะห่างที่ต้องการให้มีการแจ้งเตือน (หน่วยเป็นเซนติเมตร)

WebServer server(80);
Servo servo;

float distance = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis;
int period = 80;
int servoAngle = 90;
bool servoDirection = true;
bool buzzerState = false;

void MainPage() {
  String _html_page = html_page;
  server.send(200, "text/html", _html_page);
}


void Distance() {
  String data = "[\""+String(distance)+"\",\""+String(servoAngle)+"\"]";
  server.send(200, "text/plane", data);
}

void DistanceCalculator()
{
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);

  long duration = pulseIn(EchoPin, HIGH);
  distance = duration * SoundSpeed / 2;
  
  if (distance < dangerDistance) {
    digitalWrite(BUZZER_PIN, HIGH); // เปิด buzzer เมื่อระยะห่างน้อยกว่าค่าที่กำหนด
  } else {
    digitalWrite(BUZZER_PIN, LOW); // ปิด buzzer เมื่อระยะห่างมากกว่าหรือเท่ากับค่าที่กำหนด
  }
}

void servoController() {
  currentMillis = millis();
  if (servoAngle > 165)
    servoDirection = false;
  else if (servoAngle <= 15)
    servoDirection = true;

  if (currentMillis - previousMillis >= period) {

    if (servoDirection)
      servo.write(++servoAngle);

    else if (!servoDirection)
      servo.write(--servoAngle);

    DistanceCalculator();

    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.print("\t\tServo Angle: ");
    Serial.println(servoAngle);

    previousMillis = currentMillis;
  }
}

void setBuzzerDistance() {
    if (server.arg("distance") != "") { // ตรวจสอบว่ามีการส่งค่าระยะหรือไม่
        dangerDistance = server.arg("distance").toFloat(); // แปลงค่าเป็น float
        server.send(200, "text/plain", "Danger distance set to " + String(dangerDistance) + " cm"); // ส่งกลับไปยังไคลเอนต์
    } else {
        server.send(400, "text/plain", "Distance not provided"); // ถ้าไม่มีการส่งค่า
    }
}
void setServoAngle() {
    if (server.arg("angle") != "") {
        servoAngle = server.arg("angle").toInt();
        servo.write(servoAngle);
        server.send(200, "text/plain", "Servo Angle set to " + String(servoAngle));
    } else {
        server.send(400, "text/plain", "Angle not provided");
    }
}


void setup()
{
  servo.attach(13);
  servo.write(servoAngle);
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  WiFi.mode(WIFI_STA);    
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    Serial.print(".");
    delay(500);
  }
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  delay(7000);
  server.on("/", MainPage);
  server.on("/readDistance", Distance);
  server.on("/setBuzzerDistance", setBuzzerDistance);
  server.on("/setAngle", setServoAngle); 

  server.begin();
}

void loop()
{
  server.handleClient();
  servoController();
}