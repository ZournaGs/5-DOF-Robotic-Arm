#include <Servo.h>
Servo servoA,servoB1,servoB2,servoC,servoD,servoF;
enum ServoName {
    SERVO_A,
    SERVO_B1,
    SERVO_B2,
    SERVO_C,
    SERVO_D,
    SERVO_F
};

void SetAngle(Servo &servo,double theta,ServoName name) {
      if (theta>=0 && theta<=180){
        if(name==SERVO_B1 || name==SERVO_B2){
          servo.write(theta);
        }else{
          servo.write(theta);
          delay(500);
        }
      }else{
        Serial.print("Servo ");
        Serial.print(name);
        Serial.println(" got invalid angle!");
      }
      
    }

void SetAngleSup(Servo &servo,double theta,ServoName name) {
      SetAngle(servo,180-theta,name);
    }

void HomeArm (){
  //servoA.write(90);
  SetAngle(servoA,90,SERVO_A);
  //servoB1.write(38);
  SetAngle(servoB1,38,SERVO_B1);
  //servoB2.write(180-38);
  SetAngleSup(servoB2,38,SERVO_B2);
  //servoC.write(72);
  SetAngle(servoC,72,SERVO_C);
  //servoD.write(76);
  SetAngle(servoD,76,SERVO_D);
  //servoF.write(60);//open
  SetAngle(servoF,0,SERVO_F);
  //servoF.write(0);//close
}
void RestArm(){
  HomeArm();
  SetAngle(servoD,76+60,SERVO_D);

}





void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servoA.attach(3);
  servoB1.attach(4);
  servoB2.attach(5);
  servoC.attach(6);
  servoD.attach(7);
  servoF.attach(9);
  RestArm();
  SetAngle(servoD,180,SERVO_D);

}

void loop() {
  // put your main code here, to run repeatedly:

}
