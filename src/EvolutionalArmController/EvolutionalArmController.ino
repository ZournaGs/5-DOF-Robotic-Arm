#include <Servo.h>
#include <math.h>

//Measured values
#define L1 8
#define L2 7.6
#define L3 18.5
#define Y 5
#define phi 60
//End Measured values

//Globals
String move = "";
Servo servoA, servoB1, servoB2, servoC, servoD, servoF;
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
  double thetas[3], omega;

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

  double FindBestPhiEA(double x, double y) {
    const int POP = 60;
    const int GEN = 120;

    const double PHI_MIN = 10.0;
    const double PHI_MAX = 90.0;

    double population[POP];
    double fitness[POP];

    double bestPhi = PHI_MIN;
    double bestFitness = -1000000000.0;
    bool foundValid = false;

    for (int i = 0; i < POP; i++) {
      population[i] = PHI_MIN + ((double)random(0, 10001) / 10000.0) * (PHI_MAX - PHI_MIN);
    }

    for (int g = 0; g < GEN; g++) {

      for (int i = 0; i < POP; i++) {
        fitness[i] = FitnessPhi(x, y, population[i]);

        if (fitness[i] > bestFitness) {
          bestFitness = fitness[i];
          bestPhi = population[i];
        }

        if (fitness[i] >= 0) {
          foundValid = true;
        }
      }

      for (int i = 0; i < POP; i++) {
        int p1 = Tournament(fitness, POP);
        int p2 = Tournament(fitness, POP);

        double child = (population[p1] + population[p2]) / 2.0;

        double mutation = ((double)random(-1000, 1001) / 1000.0) * 2.0;
        child += mutation;

        if (child < PHI_MIN) child = PHI_MIN;
        if (child > PHI_MAX) child = PHI_MAX;

        population[i] = child;
      }
    }

    if (foundValid) {
      CalculateThetasWithPhi(x, y, bestPhi);
      return bestPhi;
    } else {
      Serial.println("No phi in [30,60] gives valid servo angles!");

      thetas[0] = 38;
      thetas[1] = 72;
      thetas[2] = 76;

      return -1;
    }
  }

  double FitnessPhi(double x, double y, double candidatePhi) {
    double tempTheta[3];

    if (!CalculateThetasWithPhiTemp(x, y, candidatePhi, tempTheta)) {
      return -1000000.0;
    }

    double penalty = 0.0;
    bool valid = true;

    for (int i = 0; i < 3; i++) {
      if (tempTheta[i] < 0) {
        penalty += abs(tempTheta[i]) * 1000.0;
        valid = false;
      } 
      else if (tempTheta[i] > 180) {
        penalty += abs(tempTheta[i] - 180.0) * 1000.0;
        valid = false;
      }
    }

    if (!valid) {
      return -1000000.0 - penalty;
    }

    // Αν οι γωνίες είναι έγκυρες, προτιμάται το μεγαλύτερο phi.
    return candidatePhi;
  }

  bool CalculateThetasWithPhiTemp(double x, double y, double candidatePhi, double result[3]) {
    double phi_rad = deg2rad(candidatePhi);

    double C2 = (
      x * x + y * y - L1 * L1 - L2 * L2 + L3 * L3
      - 2 * L3 * (x * cos(phi_rad) + y * sin(phi_rad))
    ) / (2 * L1 * L2);

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

    result[0] = ConvertAngle(theta1, 38);
    result[1] = ConvertAngle(theta2, 72);
    result[2] = ConvertAngle(theta3, 76);

    return true;
  }

  void CalculateThetasWithPhi(double x, double y, double candidatePhi) {
    double result[3];

    if (CalculateThetasWithPhiTemp(x, y, candidatePhi, result)) {
      thetas[0] = result[0];
      thetas[1] = result[1];
      thetas[2] = result[2];

      Serial.print("Best phi = ");
      Serial.println(candidatePhi);

      Serial.println("Real thetas with best phi:");
      PrintThetas();
    } else {
      Serial.println("No valid phi found!");
    }
  }

  int Tournament(double fitness[], int size) {
    int a = random(0, size);
    int b = random(0, size);

    if (fitness[a] > fitness[b]) {
      return a;
    } else {
      return b;
    }
  }

  double ConvertAngle(double theta, double offset) {
    if (theta >= 0) {
      return theta + offset;
    } else {
      return offset - theta;
    }
  }

  double GetOmega(double x, double z) {
    if (z >= 0 && z <= 9.1875) {
      return rad2deg(atan(z / x)) + 90;
    } 
    else if (z < 0 && z >= -12.25) {
      return rad2deg(atan(z / x) + M_PI) + 90;
    } 
    else {
      Serial.println("Arm(z) out of bounds!!!");
      return 90;
    }
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

//functions

void RestArm() {
  SetAngle(servoA, 90, SERVO_A);

  SetAngle(servoB1, 30, SERVO_B1);
  SetAngleSup(servoB2, 30, SERVO_B2);

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

void SetAngle(Servo &servo, double theta, ServoName name) {
  if (theta >= 0 && theta <= 180) {
    servo.write(theta);

    if (!(name == SERVO_B1 || name == SERVO_B2)) {
      delay(100);
    }
  } 
  else {
    Serial.print("Servo ");
    Serial.print(name);
    Serial.println(" got invalid angle!");
  }
}

void SetAngleSup(Servo &servo, double theta, ServoName name) {
  SetAngle(servo, 180 - theta, name);
}

void SetPosition(InvKin &invkin) {
  SetAngle(servoB1, invkin.GetTheta1(), SERVO_B1);
  SetAngleSup(servoB2, invkin.GetTheta1(), SERVO_B2);
  SetAngle(servoC, invkin.GetTheta2(), SERVO_C);
  SetAngle(servoD, invkin.GetTheta3(), SERVO_D);
}

void SerialCom() {
  if (Serial.available()) {
    move = Serial.readStringUntil('\n');
    Serial.print("Arduino received: ");
    Serial.println(move);
  } else {
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

  randomSeed(analogRead(A0));

  InvKin invkin;

  for (double i = 0; i < 25; i++) {
    Serial.print("x=");
    Serial.println(i+9);

    double bestPhi = invkin.FindBestPhiEA(i+9, Y);

    if (bestPhi != -1) {
      SetPosition(invkin);
    } else {
      Serial.println("Skipping movement for this x.");
    }
  }
}

void loop() {

}