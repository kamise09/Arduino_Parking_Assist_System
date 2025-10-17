const int pinLED = 7;
const int pinBuz = 8;
const int pinIR = A0;


void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuz, OUTPUT);
  pinMode(pinIR, INPUT);
  Serial.begin(9600);

  digitalWrite(pinLED, HIGH);
  tone(pinBuz, 3000);
  delay(5000);
  digitalWrite(pinLED, LOW);
  noTone(pinBuz);

}

void loop() {
  int adc = analogRead(pinIR);		// ADC 0~1023
	double voltage = adc * (5.0 / 1023.0);

  float distanceCM = 2076.0 / (adc - 11.0);
	if (distanceCM < 4) distanceCM = 4;
	if (distanceCM > 30) distanceCM = 30;
	float distanceMM = distanceCM * 10.0;

	// Serial 출력
	Serial.print("ADC: ");
	Serial.print(adc);
	Serial.print("\tVoltage: ");
	Serial.print(voltage, 2);
	Serial.print(" V\tDistance: ");
	Serial.print(distanceMM, 1);
	Serial.println(" mm");

	delay(200); 
}
