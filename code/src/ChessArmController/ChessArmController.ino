#include <Servo.h>
#include <math.h>
#define L1 7.7
#define L2 7.7
#define L3 10.5
#define Y 0

String move = "";
Servo servoA,servoB1,servoB2,servoC,servoD,servoF;

class InvKin {
private:
    double thetas[3],omega;

public:

    double GetTheta1(){
      return thetas[0];
    }
    double GetTheta2(){
      return thetas[1];
    }
    double GetTheta3(){
      return thetas[2];
    }

    void FindThetas(double x, double y) {
        double xp, yp, r, a, b, C2, S2;

        xp = x + L3;
        yp = y;

        r = sqrt(xp*xp + yp*yp);
        if(r>=abs(L3-(L1+L2)) && r<=(L1+L2+L3)){
          a = atan2(yp, xp);

        C2 = (r*r - L1*L1 - L2*L2) / (2*L1*L2);
        C2 = constrain(C2, -1.0, 1.0);//Avoid floating point errors

        S2 = sqrt(1 - C2*C2);

        b = atan2(L2*S2, (L1 + L2*C2));

        thetas[0] = rad2deg(a - b);
        thetas[1] = rad2deg(atan2(S2, C2));
        thetas[2] = -90 - (thetas[0] + thetas[1]);
        }else{
          Serial.print("Arm(r) out of bounds!!!");
          Serial.println(r);

        }
        
    }

    double GetOmega(double x, double z){
      if(z>=0 && z<=9.1875){
        return atan(z/x);
      }else if (z<0 && z>=-12.25){
        return atan(z/x)+M_PI;
      }else{
        Serial.println("Arm(z) out of bounds!!!");
      }
  }

    double rad2deg(double rad) {
        return rad * 180.0 / M_PI;
  }

    void PrintThetas(){
      for(int i=0;i<3;i++){
    Serial.println(thetas[i]);
    }

  }
};

//functions
void HomeArm (){
  servoA.write(90);
  servoB1.write(38);
  servoB2.write(180-38);
  servoC.write(72);
  servoD.write(76);
  servoF.write(30);//open
  //servoF.write(0);//close
}
void RestArm(){
  HomeArm();
  SetAngle(servoD,76+60,"servoD");

}
void GripOpen(){
  servoF.write(30);//open
}

void GripClose(){
  servoF.write(0);//close
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
  servoA.attach(3);
  servoB1.attach(4);
  servoB2.attach(5);
  servoC.attach(6);
  servoD.attach(7);
  servoF.attach(8);
  RestArm();
  SerialCom();
  /*
  InvKin invkin;
  invkin.FindThetas(15,Y);
  //invkin.PrintThetas();
  Serial.println(invkin.GetTheta1());
  Serial.println(invkin.GetTheta2());
  Serial.println(invkin.GetTheta3());
  */
}

void loop() {
  


}