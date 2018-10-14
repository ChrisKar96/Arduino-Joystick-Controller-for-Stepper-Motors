const int SW_pin = 32;
const int X_pin = A13;
const int Y_pin = A15;

void setup(){
  pinMode(SW_pin,INPUT);
  digitalWrite(SW_pin,HIGH);
  Serial.begin(115200);
}
void loop() {
  Serial.print("Switch:  ");
  Serial.print(digitalRead(SW_pin));
  Serial.print("\n");
  Serial.print("X-axis: ");
  Serial.print(analogRead(X_pin));
  Serial.print("\n");
  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_pin));
  Serial.print("\n\n");
  delay(500);
}
