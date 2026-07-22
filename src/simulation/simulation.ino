#include <Servo.h>
#include <math.h>

// Measured values
#define L1 6.824
#define L2 6.701
#define L3 10.7
#define Y 5
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
  double thetas[3];

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

  double FindBestPhi(double x, double y) {
    const double PHI_MIN = 10.0;
    const double PHI_MAX = 90.0;
    const double STEP = 1;

    double result[3];

    for (double candidatePhi = PHI_MAX; candidatePhi >= PHI_MIN; candidatePhi -= STEP) {

      if (CalculateThetasWithPhiTemp(x, y, candidatePhi, result)) {

        if (
          result[0] >= 0 && result[0] <= 180 &&
          result[1] >= 0 && result[1] <= 180 &&
          result[2] >= 0 && result[2] <= 180
        ) {
          thetas[0] = result[0];
          thetas[1] = result[1];
          thetas[2] = result[2];

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

    double S2 = sqrt(1 - C2 * C2);

    double theta2 = rad2deg(atan2(S2, C2));

    double theta1 = rad2deg(atan2(
      (y - L3 * sin(phi_rad)) * (L1 + L2 * C2)
      - (x - L3 * cos(phi_rad)) * L2 * S2,

      (x - L3 * cos(phi_rad)) * (L1 + L2 * C2)
      + (y - L3 * sin(phi_rad)) * L2 * S2
    ));

    double theta3 = candidatePhi - (theta1 + theta2);

    result[0] = ConvertAngle(theta1, 86-90);
    result[1] = ConvertAngle(theta2, 88);
    result[2] = ConvertAngle(theta3, 94);

    return true;
  }

  double ConvertAngle(double theta, double offset) {
   /* if (theta >= 0) {
      return theta + offset;
    } else if(theta < 0) {
      return offset - theta;
    }else{
      Serial.println("Invalid angle for converting!");
    }*/
    return theta+offset;
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
      delay(100);
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
  SetAngle(servoB1, invkin.GetTheta1(), SERVO_B1);
  SetAngleSup(servoB2, invkin.GetTheta1(), SERVO_B2);
  SetAngle(servoC, invkin.GetTheta2(), SERVO_C);
  SetAngle(servoD, invkin.GetTheta3(), SERVO_D);
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
  delay(2000);

  InvKin invkin;
  //Simulation
  /*
  for (double x = 9; x <= 33; x++) {
    Serial.print("x=");
    Serial.println(x);

    double bestPhi = invkin.FindBestPhi(x, Y);

    if (bestPhi != -1) {
      SetPosition(invkin);
      delay(1000);
    } else {
      Serial.println("Skipping movement for this x.");
    }
  }*/

  double bestPhi = invkin.FindBestPhi(10, 5);

    if (bestPhi != -1) {
      SetPosition(invkin);
      delay(1000);
    } else {
      Serial.println("Skipping movement for this x.");
    }


}



void loop() {

}