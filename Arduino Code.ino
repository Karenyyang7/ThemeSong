int fsrPin1 = A0;
int fsrPin2 = A1;

void setup() {
  Serial.begin(9600);
}

void loop() {
  int v1 = analogRead(fsrPin1);
  int v2 = analogRead(fsrPin2);

  // send as: 123,456\n
  Serial.print(v1);
  Serial.print(",");
  Serial.println(v2);

  delay(50);
}
