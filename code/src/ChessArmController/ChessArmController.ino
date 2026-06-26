#include <Servo.h>
#include <math.h>
#define L1 7.7
#define L2 7.7
#define L3 10.5
String move = "";
Servo servoA,servoB1,servoB2,servoC,servoD,servoF;

//functions
void AttachServo(Servo servo,int dig_pin) {
      servo.attach(dig_pin);
    }

void SetAngle(Servo servo,double theta,String name) {
      if (theta>=0 && theta<=180){
        servo.write(theta);
        delay(100);
      }else{
        Serial.print("Servo ");
        Serial.print(name);
        Serial.println(" got invalid angle!");
      }
      
    }

void SetAngleSup(Servo servo,double theta,String name) {
      SetAngle(servo,180-theta,name);
    }
double rad2deg(double rad){
  return (180/M_PI)*rad;
}

void SerialCom(){
  if (Serial.available()) {
    move = Serial.readStringUntil('\n');
    Serial.print("Arduino received: ");
    Serial.println(move);
    }else{
      Serial.println("Serial communication is unavailable!");
    }
}



void setup() {
  Serial.begin(9600);
}

void loop() {
  SerialCom();

}