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
int adc1 = 200, adc2 = 200, cnt = 0, cnt2 = 0;
int rlSave = 1, clkSave = 0, clk = 0, status = 0;
int raw_adc1, raw_adc2;

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
unsigned int step_delay_us = 2400; 

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
	adc1 = 200; adc2 = 200;

	disableCoils(); // 코일 초기화
	Serial.begin(115200); // Serial 통신속도 설정

}

void loop() {
	int RL; inpIR(); clk = 0; status = 0;
	serialPrint(adc1, adc2);

	// 로직구현 좌회전 + 우회전 -
	if(adc1 < 200 || adc2 < 200){
		cnt++;
	}else{
		cnt = 0;
	}

	if(cnt == 30){
		cnt = 0; cnt2 = 0;
		warning();
		while(adc1 < 200 || adc2 < 200){
			RL = (adc2-adc1)<0 ? -1:1; clk ++;
			rotateDegrees(RL*10);
			inpIR();serialPrint(adc1, adc2);
			if(RL != rlSave){
				if(status == 0){
					status = 1;
					cnt2++; clkSave = clk;
				}else if(status == 1){
					if((clk - clkSave) <= 10){
						cnt2++; clkSave = clk;
					}
				}
			}else{
				cnt2 = 0;
			}
			rlSave = RL;
			if(cnt2 == 5) break;
		}
	}

	delay(100);
}

void inpIR(){
	raw_adc1 = analogRead(pinIR);
	raw_adc2 = analogRead(pinIR2);

	adc1 = mmPrint(raw_adc1);	
	adc2 = mmPrint(raw_adc2);
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

double mmPrint(int x){ // x: 측정된 ADC 값
  
  // 1. 최대 거리 고정 (200mm 이상)
  // ADC가 115 미만이면 (계산상 200mm 초과) 무조건 200mm로 고정
  if (x < 115) {
      return 200.0; 
  }

  // 2. 거리 변환 (x는 115 이상이므로 t는 104 이상)
  double t = (double)x - 11.0; 
  double distanceCM = 2076.0 / t; // cm 변환 수식
  
  // 3. 최소 거리 고정 (40mm 이하)
  if (distanceCM < 4.0) {
      return 40.0; // 4cm -> 40mm
  }

  // 4. 최종 변환 및 반환 (200mm를 초과하는 값은 이미 위에서 걸러졌으므로 불필요)
  double distanceMM = distanceCM * 10.0;
  return distanceMM;
}

// 거리센서 값을 adc, V, mm 형태로 Serial출력
void serialPrint(int p1, int p2){
	Serial.print("ADC: "); 
	Serial.print(raw_adc1); 
	Serial.print(", "); 
	Serial.print(raw_adc2);
	Serial.print("\tDistance: "); 
	Serial.print((double)p1, 1); 
	Serial.print(", "); 
	Serial.print((double)p2, 1);
	Serial.println(" mm");
	Serial.println("    ");
	Serial.println(cnt);
}