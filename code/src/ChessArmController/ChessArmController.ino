String move = "";

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    move = Serial.readStringUntil('\n');

    Serial.print("Arduino received: ");
    Serial.println(move);

    // εδώ μετά:
    // executeMove(move);
  }
}