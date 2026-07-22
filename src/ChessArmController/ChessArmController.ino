#include <Servo.h>
#include <math.h>

//Measured values
#define L1 8
#define L2 7.6
#define L3 18.5//10.5 without gripper
#define Y 10
#define phi 45
//End Measured values

//Globals
String move = "";
Servo servoA,servoB1,servoB2,servoC,servoD,servoF;
//End Globals

enum ServoName {
    SERVO_A,
    SERVO_B1,
    SERVO_B2,
    SERVO_C,
    SERVO_D,
    SERVO_F
};

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
        
        /*Serial.println("Trig values:");
        Serial.println(cos(deg2rad(phi)));
        Serial.println(sin(deg2rad(phi)));
        */
        /* //Model1
        xp = x-L3*cos(deg2rad(phi));
        yp = y-L3*sin(deg2rad(phi));

        r = sqrt(xp*xp + yp*yp);
        if(r>=abs(L1-L2) && r<=(L1+L2)){
          Serial.print("r=");
          Serial.println(r);
         
          a = atan2(yp, xp);

        C2 = (r*r - L1*L1 - L2*L2) / (2*L1*L2);
        C2 = constrain(C2, -1.0, 1.0);//Avoid floating point errors

        S2 = sqrt(1 - C2*C2);

        b = atan2(L2*S2, (L1 + L2*C2));

        thetas[0] = rad2deg(a - b);
        thetas[1] = rad2deg(atan2(S2, C2));
        thetas[2] = phi - (thetas[0] + thetas[1]);
        */
        //Model 2
        double phi_rad=deg2rad(phi);
        C2=(x*x+y*y-L1*L1-L2*L2+L3*L3-2*L3*(x*cos(phi_rad)+y*sin(phi_rad)))/(2*L1*L2);
        Serial.print("C2 is ");
        Serial.println(C2);
        if((C2>=-1 && C2<=1) ){
          if((C2!=-1 || L1!=L2)){
          S2=sqrt(1-C2*C2);
          Serial.print("S2 is ");
          Serial.println(S2);
          thetas[1]=rad2deg(atan2(S2,C2));
          thetas[0]=rad2deg(atan2(
                    (y - L3*sin(phi_rad)) * (L1 + L2*C2)
                  - (x - L3*cos(phi_rad)) * L2*S2,

                    (x - L3*cos(phi_rad)) * (L1 + L2*C2)
                  + (y - L3*sin(phi_rad)) * L2*S2
                  ));
          thetas[2]=rad2deg(phi_rad)-(thetas[0]+thetas[1]);
          Serial.println("Theoritical thetas:");
          PrintThetas();

          //Converting theta's from theoretical model
          thetas[0]=thetas[0]+abs(thetas[0]-38);//Servos B1,B2
          thetas[1]=thetas[1]+abs(thetas[1]-72);//Servo C
          thetas[2]=thetas[2]+abs(thetas[2]-76);// Servo D
          Serial.println("Real thetas:");
          PrintThetas();

          }else{
            Serial.print("Arm out of bounds!!!");
            //Home position for not breaking the arm
            thetas[0]=38;//Servos B1,B2
            thetas[1]=72;//Servo C
            thetas[2]=76;// Servo D
          }

        }else{
          Serial.println("Cosine is out of bounds!!!");
      }
        
        
    }

    double ConvertAngle(double theta,double offset){
      if(theta>=0){
        return theta + offset;
      }else if (theta<0){
        return offset - theta;
      }else{
        Serial.print(theta);
        Serial.println("cannot be converted!");
      }
    }

    double GetOmega(double x, double z){
      if(z>=0 && z<=9.1875){
        return rad2deg(atan(z/x))+90;//+90 offset from theoritical model
      }else if (z<0 && z>=-12.25){
        return rad2deg(atan(z/x)+M_PI)+90;//same
      }else{
        Serial.println("Arm(z) out of bounds!!!");
      }
  }

    double rad2deg(double rad) {
        return rad * 180.0 / M_PI;
  }
    double deg2rad(double deg){
      return deg * M_PI/180;
    }

    void PrintThetas(){
      for(int i=0;i<3;i++){
    Serial.println(thetas[i]);
    }
  }
};//End InvKin

//functions
void RestArm (){
  //servoA.write(90);
  SetAngle(servoA,90,SERVO_A);
  //servoB1.write(38);
  SetAngle(servoB1,30,SERVO_B1);
  //servoB2.write(180-38);
  SetAngleSup(servoB2,30,SERVO_B2);
  //servoC.write(72);
  SetAngle(servoC,80,SERVO_C);
  //servoD.write(76);
  SetAngle(servoD,136,SERVO_D);
  //servoF.write(60);//open
  SetAngle(servoF,0,SERVO_F);
  //servoF.write(0);//close
}

void GripOpen(){
  //servoF.write(60);//open
  SetAngle(servoF,0,SERVO_F);
}

void GripClose(){
  //servoF.write(40);//close
  SetAngle(servoF,160,SERVO_F);
}

void SetAngle(Servo &servo,double theta,ServoName name) {
      if (theta>=0 && theta<=180){
        if(name==SERVO_B1 || name==SERVO_B2){
          servo.write(theta);
        }else{
          servo.write(theta);
          delay(100);
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

void SetPosition(InvKin &invkin){
  SetAngle(servoB1,invkin.GetTheta1(),SERVO_B1);
  SetAngleSup(servoB2,invkin.GetTheta1(),SERVO_B2);
  SetAngle(servoC,invkin.GetTheta2(),SERVO_C);
  SetAngle(servoD,invkin.GetTheta3(),SERVO_D);
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
//End functions



void setup() {
  Serial.begin(9600);
  servoA.attach(3);
  servoB1.attach(4);
  servoB2.attach(5);
  servoC.attach(6);
  servoD.attach(7);
  servoF.attach(9);
  RestArm();
  SerialCom();
  InvKin invkin;
  invkin.FindThetas(20,Y);//x=1..26,Y=5 are valid
  SetPosition(invkin);
  /*delay(2000);
  RestArm();*/
  
}

void loop() {
  


}