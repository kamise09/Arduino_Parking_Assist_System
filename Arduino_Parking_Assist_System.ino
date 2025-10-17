const int pinLED = 7;
const int pinBuz = 8;
int md = 0;

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuz, OUTPUT);

  digitalWrite(pinLED, HIGH);
  tone(pinBuz, 3000);
  delay(5000);
  digitalWrite(pinLED, LOW);
  noTone(pinBuz);
  
}

void loop() {


}
