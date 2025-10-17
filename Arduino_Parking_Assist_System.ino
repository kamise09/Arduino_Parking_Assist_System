const int pinLED = 7;
const int pinBuz = 8;
int md = 0;

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuz, OUTPUT);

  digitalWrite(pinLED, HIGH);
  delay(5000);
  tone(pinBuz, 1000);
  digitalWrite(pinLED, LOW);
}

void loop() {


}
