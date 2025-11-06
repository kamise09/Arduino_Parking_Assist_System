#include<math.h>

// 변수에 핀번호 할당
const int pinLED = 7; // LED핀
const int pinBuz = 8; // 부저 핀
const int pinIR = A0; // 거리센서1
const int pinIR2 = A1;// 거리센서2

// 모터 드라이버 관련 핀 
const int IN1 = 9; 
const int IN2 = 10;
const int IN3 = 11;
const int IN4 = 12;


const int8_t seq[8][4] = { // 스텝 구동을 위한 배열 
	{1, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 0, 0, 1},
	{1, 0, 0, 1}
};

long STEPS_PER_REV = 2048;
unsigned int step_delay_us = 1200;

// 필요 함수 정의
void stepOnce(int dir);
void stepN(long steps, int dir);
void rotateDegrees(float angle_deg);
void disableCoils();
void warning();
double mmPrint(int x);


void setup() {

	// pinMode 초기화
  pinMode(pinLED, OUTPUT);
  pinMode(pinBuz, OUTPUT);
  pinMode(pinIR, INPUT);
	pinMode(pinIR2, INPUT);
  pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);

	disableCoils(); // 코일 초기화
	Serial.begin(9600); // Serial 통신속도

}

void loop() {
  int adc1 = analogRead(pinIR);	
	int adc2 = analogRead(pinIR2);
	double voltage1 = adc1 * (5.0 / 1023.0);
	double voltage2 = adc2 * (5.0 / 1023.0);

	// Serial 출력
	Serial.print("ADC: "); 
	Serial.print(adc1); 
	Serial.print(", "); 
	Serial.print(adc2);
	Serial.print("\tVoltage: "); 
	Serial.print(voltage1, 2); 
	Serial.print(", "); 
	Serial.print(voltage2, 2);
	Serial.print(" V\tDistance: "); 
	Serial.print(mmPrint(adc1), 1); 
	Serial.print(", "); 
	Serial.print(mmPrint(adc2), 1);
	Serial.println(" mm");

	rotateDegrees(90.0f);
	delay(1000);
	rotateDegrees(-45.0f);
	delay(1000);

	delay(200);
}


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
	disableCoils(); 
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

void warning(){
	digitalWrite(pinLED, HIGH);
  tone(pinBuz, 3000);
  digitalWrite(pinLED, LOW);
  noTone(pinBuz);
}

double mmPrint(int x){

  float distanceCM = 2076.0 / (x - 11.0);
	if (distanceCM < 4) distanceCM = 4;
	if (distanceCM > 30) distanceCM = 30;
	float distanceMM = distanceCM * 10.0;

	return distanceMM;
}