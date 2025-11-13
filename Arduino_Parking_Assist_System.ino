#include<math.h>

const int pinLED = 7; // LED핀
const int pinBuz = 8; // 부저 핀
const int pinIR = A0; // 거리센서1 - 왼쪽 센서가 보는 방향으로
const int pinIR2 = A1;// 거리센서2 - 오른쪽 

// 모터 드라이버 관련 핀 
const int IN1 = 9; 
const int IN2 = 10;
const int IN3 = 11;
const int IN4 = 12;

// adc 
int adc1, adc2 ;
double voltage1, voltage2;

// 스텝 구동을 위한 8단계 하프스텝 테이블 
// 한 행은 스텝 1단계, 각 열은 IN1 ~ IN4
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

// 1회전당 스텝수, 각도-> 스텝 변환에 사용
long STEPS_PER_REV = 2048;
// 스텝사이 지연시간 설정, 모터속도 조절 변수
unsigned int step_delay_us = 1600; 

// 필요 함수 정의
void stepOnce(int dir);
void stepN(long steps, int dir);
void rotateDegrees(float angle_deg);
void disableCoils();
void warning();
void serialPrint(int p1, int p2);
double mmPrint(int x);
void inpIR();

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
	Serial.begin(115200); // Serial 통신속도 설정

}

void loop() {
	int RL; inpIR();
	serialPrint(adc1, adc2);
	
	// 로직구현 좌회전 + 우회전 -

	if(adc1 < 300 || adc2 < 300){
		delay(3000);
		inpIR();
		if(adc1 < 300 || adc2 < 300){
			warning();
			while(adc1 < 300 || adc2 < 300){
				inpIR();
				RL = (adc2-adc1)<0 ? -1:1;
				rotateDegrees(RL);
			}	
		}
	}

	delay(100);
}

void inpIR(){
	adc1 = mmPrint(analogRead(pinIR));	
	adc2 = mmPrint(analogRead(pinIR2));
}

// 1스텝 회전을 실행하는 함수 
void stepOnce(int dir) {
	static int idx = 0;
	idx = (idx + (dir > 0 ? 1 : -1) + 8) % 8; 

	// seq[] 배열에 대응하여 IN1~IN4 디지털 출력
	digitalWrite(IN1, seq[idx][0]);
	digitalWrite(IN2, seq[idx][1]);
	digitalWrite(IN3, seq[idx][2]);
	digitalWrite(IN4, seq[idx][3]);

	delayMicroseconds(step_delay_us);
}

// stepOnce() 함수를 N만큼 반복호출 후 코일 off 
void stepN(long steps, int dir) {
	for (long i = 0; i < steps; i++) {
		stepOnce(dir);
	}
	disableCoils(); 
}

// 각도 입력받고 회전에 필요한 스텝 수를 계산, stepN()함수에 전달
void rotateDegrees(float angle_deg) {
	int dir = (angle_deg >= 0.0f) ? +1 : -1; // 방향제어 변수
	long steps = (long) lroundf(fabsf(angle_deg) * (float)STEPS_PER_REV / 360.0f);
	stepN(steps, dir);
}

// 모든 코일 전류차단, 초기화 함수
void disableCoils() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW);
}

// 모터 동작 전 부저, LED 경고신호 작동 함수
void warning(){
	digitalWrite(pinLED, HIGH); tone(pinBuz, 3000);
	delay(1000); // 3초간 작동
  digitalWrite(pinLED, LOW); noTone(pinBuz);
}

// 거리센서 아날로그값을 mm 단위 로 변환하는 함수 
double mmPrint(int x){

  float distanceCM = 2076.0 / (x - 11.0);// cm 변환 수식
	
	// 4cm 이하면 4로, 30cm 이상이면 30으로 고정 
	if (distanceCM < 4) distanceCM = 4;
	if (distanceCM > 30) distanceCM = 30;
	float distanceMM = distanceCM * 10.0;

	return distanceMM;
}

// 거리센서 값을 adc, V, mm 형태로 Serial출력
void serialPrint(int p1, int p2){
	Serial.print("ADC: "); 
	Serial.print(analogRead(pinIR)); 
	Serial.print(", "); 
	Serial.print(analogRead(pinIR2));
	Serial.print(" V\tDistance: "); 
	Serial.print(p1, 1); 
	Serial.print(", "); 
	Serial.print(p2, 1);
	Serial.println(" mm");
}