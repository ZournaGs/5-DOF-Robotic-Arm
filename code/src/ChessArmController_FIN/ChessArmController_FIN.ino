#include <Servo.h>
#include <math.h>

// Measured values
#define L1 6.824
#define L2 6.701
#define L3 18.7
#define Y -2
#define TOLERANCE 0.5
// End measured values

String move = "";

Servo servoA, servoB1, servoB2, servoC, servoD, servoF;

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

  double GetTheta1() {
    return thetas[0];
  }

  double GetTheta2() {
    return thetas[1];
  }

  double GetTheta3() {
    return thetas[2];
  }

  double GetOmega(){
    return omega;
  }

  double FindBestPhi(double x, double y) {
    const double PHI_MIN = -90.0;
    const double PHI_MAX = 90.0;
    const double STEP = 1;

    double result[3];

    for (double candidatePhi = PHI_MIN; candidatePhi <= PHI_MAX; candidatePhi += STEP) {
      /*Serial.print("Current phi: ");
      Serial.println(candidatePhi);*/

      if (CalculateThetasWithPhiTemp(x, y, candidatePhi, result)) {

        if (
          result[0] >= 0 && result[0] <= 180 &&
          result[1] >= 0 && result[1] <= 180 &&
          result[2] >= 0 && result[2] <= 180 
        ) {
          thetas[0] = result[0];
          thetas[1] = result[1];
          thetas[2] = result[2];
           Serial.print("x: ");
           Serial.println(x);
           Serial.print("y: ");
           Serial.println(y);

          Serial.print("Best phi = ");
          Serial.println(candidatePhi);

          Serial.println("Real thetas:");
          PrintThetas();

          return candidatePhi;
        }
      }
    }

    Serial.println("No phi in range gives valid servo angles!");

    thetas[0] = 86;
    thetas[1] = 88;
    thetas[2] = 94;

    return -1;
  }

  bool CalculateThetasWithPhiTemp(double x, double y, double candidatePhi, double result[3]) {
    double phi_rad = deg2rad(candidatePhi);

    double C2 = (
      x * x + y * y - L1 * L1 - L2 * L2 + L3 * L3
      - 2 * L3 * (x * cos(phi_rad) + y * sin(phi_rad))
    ) / (2 * L1 * L2);

    //NOTE: If L1=L2 then the condition L1!=L2 && C1!=-1 should be checked
    if (C2 < -1.0 || C2 > 1.0) {
      return false;
    }

    C2 = constrain(C2, -1.0, 1.0);

    double S2 = -sqrt(1 - C2 * C2);

    double theta2 = rad2deg(atan2(S2, C2));

    double theta1 = rad2deg(atan2(
      (y - L3 * sin(phi_rad)) * (L1 + L2 * C2)
      - (x - L3 * cos(phi_rad)) * L2 * S2,

      (x - L3 * cos(phi_rad)) * (L1 + L2 * C2)
      + (y - L3 * sin(phi_rad)) * L2 * S2
    ));

    double theta3 = candidatePhi - (theta1 + theta2);
    double raw[3] = {theta1, theta2, theta3};
    if (!ForwardKinematicsCheck(x, y, raw)) {
      return false;
    }
    Serial.println("Theoretical angles:");
    Serial.print("theta1 = ");
    Serial.println(theta1);

    Serial.print("theta2 = ");
    Serial.println(theta2);

    Serial.print("theta3 = ");
    Serial.println(theta3);

    result[0] = ConvertAngle(theta1, 180);
    result[1] = ConvertAngle(theta2, 88);
    result[2] = ConvertAngle(theta3, 94);
    /*result[0]=180-theta1;
    result[1]=88-theta2;
    result[2]=90-theta3;*/

    Serial.println("Servo angles:");
    Serial.print("B1/B2 = ");
    Serial.println(result[0]);

    Serial.print("C = ");
    Serial.println(result[1]);

    Serial.print("D = ");
    Serial.println(result[2]);

    return true;
  }

  bool ForwardKinematicsCheck(double xr, double yr,double result[3]){
    double x,y,res[3];

    for(int i=0;i<3;i++){
      res[i]=deg2rad(result[i]);
    }

    x=L1*cos(res[0])+L2*cos(res[0]+res[1])+L3*cos(res[0]+res[1]+res[2]);
    y=L1*sin(res[0])+L2*sin(res[0]+res[1])+L3*sin(res[0]+res[1]+res[2]);
    


    if(abs(xr-x)<=TOLERANCE && abs(yr-y)<=TOLERANCE){
      return true;
    }else{
      return false;
    } 

  }
  double FindOmega(double z , double x){
    omega=rad2deg(atan2(z,x));
    omega=ConvertAngle(omega,90);
    if(omega>=0 && omega<=180){
      Serial.print("Omega is: ");
      Serial.println(omega);
      return omega;
    }else{
      Serial.print("Invalid omega equal to ");
      Serial.println(omega);
      return -1;
    }
  }
  

  double ConvertAngle(double angle, double offset) {
    return offset-angle ;
  }

  double rad2deg(double rad) {
    return rad * 180.0 / M_PI;
  }

  double deg2rad(double deg) {
    return deg * M_PI / 180.0;
  }

  void PrintThetas() {
    for (int i = 0; i < 3; i++) {
      Serial.println(thetas[i]);
    }
  }
};

// Functions

void SetAngle(Servo &servo, double theta, ServoName name) {
  if (theta >= 0 && theta <= 180) {
    servo.write(theta);

    if (!(name == SERVO_B1 || name == SERVO_B2)) {
      delay(500);
    }
  } else {
    Serial.print("Servo ");
    Serial.print(name);
    Serial.println(" got invalid angle!");
  }
}

void SetAngleSup(Servo &servo, double theta, ServoName name) {
  SetAngle(servo, 180 - theta, name);
}

void RestArm() {
  SetAngle(servoA, 90, SERVO_A);

  SetAngle(servoB1, 90, SERVO_B1);
  SetAngleSup(servoB2, 90, SERVO_B2);

  SetAngle(servoC, 80, SERVO_C);
  SetAngle(servoD, 136, SERVO_D);

  SetAngle(servoF, 0, SERVO_F);
}

void GripOpen() {
  SetAngle(servoF, 0, SERVO_F);
}

void GripClose() {
  SetAngle(servoF, 160, SERVO_F);
}

void SetPosition(InvKin &invkin) {
  SetAngle(servoA, invkin.GetOmega(), SERVO_A);
  SetAngle(servoB1, invkin.GetTheta1(), SERVO_B1);
  SetAngleSup(servoB2, invkin.GetTheta1(), SERVO_B2);
  SetAngle(servoC, invkin.GetTheta2(), SERVO_C);
  SetAngle(servoD, invkin.GetTheta3(), SERVO_D);
}

void GoTo(InvKin &invkin, double x , double z){

  double bestPhi = invkin.FindBestPhi(x, Y);//x=15...x=32,y=0->x=14...32,y=-3
  double omega = invkin.FindOmega(z, x);//z=9.1875...z=-12.25

    if (bestPhi != -1 && omega!= -1) {
      SetPosition(invkin);
      delay(1000);
    } else {
      Serial.println("Skipping movement for this position.");
    }

}

void setup() {
  Serial.begin(9600);

  servoA.attach(3);
  servoB1.attach(4);
  servoB2.attach(5);
  servoC.attach(6);
  servoD.attach(7);
  servoF.attach(9);

  RestArm();
  delay(1500);

  InvKin invkin;
  GoTo(invkin,15,10);//x=15..32 when y=-1

}



void loop() {

}