#include <Servo.h>


class ServoPair {
  private:
    int potPin;
    int potValue;
  
  protected:
  Servo servo;
  int angle;

  public:
    void AttachServo(int dig_pin) {
      servo.attach(dig_pin);
    }

    void AttachPot(int an_pin) {
      potPin = an_pin;
    }

    int potValueGet() {
      potValue = analogRead(potPin);
      return potValue;
    }

    void SetAngle() {
      angle = map(potValueGet(), 0, 1023, 0, 180);
      servo.write(angle);
      delay(10);
    }
    void SetAngle(int in_deg,int fin_deg){
      angle = map(potValueGet(), 0, 1023, in_deg, fin_deg);
      servo.write(angle);
      delay(10);
    }
} servo_pair[5];

class doubleServoPair: public ServoPair{
  public:

  void SetAngle(doubleServoPair &d1, doubleServoPair &d2){
    d1.angle=map(d1.potValueGet(), 0, 1023, 0, 180);
    d1.servo.write(d1.angle);
    d2.servo.write(180-d1.angle);
    delay(10);
  }
  
}double_servo_pair[2];

void setup() {
  //Servo A(D3)
  servo_pair[0].AttachServo(3);
  servo_pair[0].AttachPot(A0);

  //Servo's B1-B2(D4-D5)
  double_servo_pair[0].AttachServo(4);
  double_servo_pair[1].AttachServo(5);
  double_servo_pair[0].AttachPot(A1);

  //Servo C(D6)
  servo_pair[2].AttachServo(6);
  servo_pair[2].AttachPot(A2);

  //Servo D(D7)
  servo_pair[3].AttachServo(7);
  servo_pair[3].AttachPot(A3);

  //Servo F(D8)
  servo_pair[4].AttachServo(8);
  servo_pair[4].AttachPot(A4);
}

void loop() {
  servo_pair[0].SetAngle();
  double_servo_pair[0].SetAngle(double_servo_pair[0],double_servo_pair[1]);
  servo_pair[2].SetAngle();
  servo_pair[3].SetAngle();
  servo_pair[4].SetAngle();
}