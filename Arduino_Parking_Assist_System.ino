#include<math.h>

const int pinLED = 7;
const int pinBuz = 8;
const int pinIR = A0;
const int IN1 = 9;
const int IN2 = 10;
const int IN3 = 11;
const int IN4 = 12;

const int8_t seq[8][4] = {
	{1, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 0, 0, 1},
	{1, 0, 0, 1}
};

// 한 바퀴 스텝 수 (half-step 기준). 필요하면 2048 ↔ 4096 사이에서 보정.
long STEPS_PER_REV = 2048;

// 스텝 간 지연(us). 값이 작을수록 빠르게 회전. 너무 작으면 토크 부족/스텝로스 가능.
unsigned int step_delay_us = 1200;

static int seqIndex = 0; // 시퀀스 위치 기억

void stepOnce(int dir) {
	static int idx = 0;
	idx = (idx + (dir > 0 ? 1 : -1) + 8) % 8;

	digitalWrite(IN1, seq[idx][0]);
	digitalWrite(IN2, seq[idx][1]);
	digitalWrite(IN3, seq[idx][2]);
	digitalWrite(IN4, seq[idx][3]);
	delayMicroseconds(step_delay_us);
}

void stepN(long steps, int dir) {
	for (long i = 0; i < steps; i++) {
		stepOnce(dir);
	}
	disableCoils(); // 멈출 때 발열 줄이려면 코일 오프
}

void rotateDegrees(float angle_deg) {
	int dir = (angle_deg >= 0.0f) ? +1 : -1;
	long steps = (long) lroundf(fabsf(angle_deg) * (float)STEPS_PER_REV / 360.0f);
	stepN(steps, dir);
}

void disableCoils() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
}

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuz, OUTPUT);
  pinMode(pinIR, INPUT);
  pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

	disableCoils();
	
	
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

	rotateDegrees(90.0f);
	delay(1000);
	rotateDegrees(-45.0f);
	delay(1000);

}
