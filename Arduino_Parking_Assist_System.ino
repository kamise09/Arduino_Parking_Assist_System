const pinLED = 7;
const pinBuz = 8;
int md = 0;

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuz, OUTPUT);
}

void loop() {
  
  if(md == 0){
    digitalWrite(pinLED, HIGH);
    tone(pinBuz, 1000);
    delay(1000);
    digitalWrite(pinLED, LOW);
    md = 1;
  }

}
