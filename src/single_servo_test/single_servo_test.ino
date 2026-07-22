#include <Servo.h>

Servo myServo;

int potPin = A0;
int potValue;
int angle;

void setup() {
  myServo.attach(3);   // servo στο D3
}

void loop() {

  potValue = analogRead(potPin);

  angle = map(potValue, 0, 1023, 0, 180);

  myServo.write(angle);

  delay(10);
}