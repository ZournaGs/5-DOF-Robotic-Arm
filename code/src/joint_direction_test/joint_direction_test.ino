#include <Servo.h>

// Same offsets as in the main sketch (these represent theta = 0 for each joint)
#define OFFSET_B 86
#define OFFSET_C 88
#define OFFSET_D 94

#define TEST_DELTA 20   // how many degrees to move away from center for the test
#define PAUSE_MS 3000   // time to hold each position so you can observe it

Servo servoA, servoB1, servoB2, servoC, servoD, servoF;

void SetAngle(Servo &servo, double theta) {
  if (theta >= 0 && theta <= 180) {
    servo.write(theta);
  } else {
    Serial.print("INVALID ANGLE: ");
    Serial.println(theta);
  }
}

void SetAngleSup(Servo &servo, double theta) {
  SetAngle(servo, 180 - theta);
}

// Move joint B (the differential B1/B2 pair) to a given "command" value
void MoveB(double cmd) {
  SetAngle(servoB1, cmd);
  SetAngleSup(servoB2, cmd);
}

void GoNeutral() {
  MoveB(OFFSET_B);
  SetAngle(servoC, OFFSET_C);
  SetAngle(servoD, OFFSET_D);
  Serial.println("=== NEUTRAL (B=86, C=88, D=94 -> theta1=theta2=theta3=0) ===");
  Serial.println("Look at the arm now and remember this as the reference position.");
  delay(PAUSE_MS);
}

void TestJoint(const char* name, void (*moveFunc)(double), double center, double delta) {
  Serial.print("--- Testing joint ");
  Serial.print(name);
  Serial.println(" ---");

  Serial.print("Moving ");
  Serial.print(name);
  Serial.print(" to +");
  Serial.print(delta);
  Serial.println(" from neutral. Watch which way the arm tip moves, then note it down.");
  moveFunc(center + delta);
  delay(PAUSE_MS);

  Serial.println("Back to neutral.");
  moveFunc(center);
  delay(PAUSE_MS);

  Serial.print("Moving ");
  Serial.print(name);
  Serial.print(" to -");
  Serial.print(delta);
  Serial.println(" from neutral. Watch which way the arm tip moves, then note it down.");
  moveFunc(center - delta);
  delay(PAUSE_MS);

  Serial.println("Back to neutral.");
  moveFunc(center);
  delay(PAUSE_MS);
}

// Wrapper functions matching the (double) signature needed by TestJoint
void MoveC(double cmd) { SetAngle(servoC, cmd); }
void MoveD(double cmd) { SetAngle(servoD, cmd); }

void setup() {
  Serial.begin(9600);

  servoA.attach(3);
  servoB1.attach(4);
  servoB2.attach(5);
  servoC.attach(6);
  servoD.attach(7);
  servoF.attach(9);

  delay(500);

  GoNeutral();

  TestJoint("B (theta1 / base joint)", MoveB, OFFSET_B, TEST_DELTA);
  GoNeutral();

  TestJoint("C (theta2 / elbow joint)", MoveC, OFFSET_C, TEST_DELTA);
  GoNeutral();

  TestJoint("D (theta3 / wrist joint)", MoveD, OFFSET_D, TEST_DELTA);
  GoNeutral();

  Serial.println("=== TEST COMPLETE ===");
  Serial.println("Please report, for each joint, whether +delta moved the arm tip");
  Serial.println("up/down/left/right/forward/back relative to neutral.");
}

void loop() {
  // nothing here, test runs once in setup()
}
