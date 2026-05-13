#include <SoftwareSerial.h>
#include <AFMotor.h>
#define TRIG_PIN A5
#define ECHO_PIN A4
#define BT_STATE A3
SoftwareSerial BTSerial(9, 10);
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);
char BTdata = 'S';
char mode = 'M'; 
long duration;
float distance;
bool blockForward = false;
#define MANUAL_SPEED 200
#define AUTO_SPEED 200
float manualSetDistance = 25;
float autoSetDistance = 30;
int turning_delay = 350;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BT_STATE, INPUT);
  setMotorSpeed(MANUAL_SPEED);
  Serial.println("System Ready");
}
void loop() {
  read_BT();
  if (digitalRead(BT_STATE) == LOW) {
    Stop();
    return;
  }
  if (BTdata == '1') {
    Stop();
    mode = 'M';
    setMotorSpeed(MANUAL_SPEED);
    BTdata = 'S';
    Serial.println("Manual Mode");
    return;
  }
  if (BTdata == '2') {
    Stop();
    mode = 'A';
    setMotorSpeed(AUTO_SPEED);
    BTdata = 'S';
    Serial.println("Auto Mode Started");
    return;}
  if (BTdata == 'S') {
    Stop();}
  if (mode == 'M') {
    runManual();
  } else {
    runAuto();}
}
void runManual() {
  read_Ultrasonic();
  if (BTdata == 'F' && distance > 0 && distance <= manualSetDistance) {
    Stop();
    blockForward = true;
    return;}
  if (BTSerial.available()) {
    blockForward = false;}
  if (BTdata == 'F' && !blockForward) {
    Move_Forward();}
  else if (BTdata == 'B') {
    Move_Backward();}
  else if (BTdata == 'L') {
    Turn_Left();}
  else if (BTdata == 'R') {
    Turn_Right();}
  else if (BTdata == 'S') {
    Stop();}
}
void runAuto() {
  if (BTdata == 'S') {
    Stop();
    return;}
  read_Ultrasonic();
  if (distance > 0 && distance <= autoSetDistance) {
    avoidObstacle();
  } else {
    Move_Forward();}
}
bool checkInterrupt() {
  read_BT();
  if (BTdata == 'S') {
    Stop();
    return true;}
  if (BTdata == '1' || BTdata == '2') {
    return true;}
  return false;
}
void read_Ultrasonic() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH, 15000);
  if (duration == 0) distance = 999;
  else distance = duration * 0.034 / 2;
}
void avoidObstacle() {
  Stop();
  delay(200);
  if (checkInterrupt()) return;
  Move_Backward();
  delay(turning_delay);
  if (checkInterrupt()) return;
  Stop();
  delay(200);
  if (checkInterrupt()) return;
  Turn_Right();
  delay(turning_delay);
  if (checkInterrupt()) return;
  Stop();
  delay(200);
  if (checkInterrupt()) return;
  read_Ultrasonic();
  if (distance <= autoSetDistance) {
    Turn_Left();
    delay(turning_delay * 2);
    if (checkInterrupt()) return;
    Stop();
    delay(200);
    if (checkInterrupt()) return;
    read_Ultrasonic();
    if (distance <= autoSetDistance) {
      Turn_Left();
      delay(turning_delay);
      if (checkInterrupt()) return;
      Stop();
      delay(200);}}
}
void setMotorSpeed(int spd) {
  motor1.setSpeed(spd);
  motor2.setSpeed(spd);
  motor3.setSpeed(spd);
  motor4.setSpeed(spd);}
void Move_Forward() {
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);}
void Move_Backward() {
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);}
void Turn_Right() {
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(BACKWARD);}
void Turn_Left() {
  motor1.run(FORWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(FORWARD);}
void Stop() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);}
void read_BT() {
if (BTSerial.available()) {
  BTdata = BTSerial.read();
  Serial.print("Received: ");
  Serial.println(BTdata);}
}