//付録G茎伸長量計測プログラム
//A相を2番ピン,B相を3番ピンに接続
#include<Servo.h>
//数列を保存する変数を準備
Servo servo1;

int valEncA;
int valEncB;

int const EncA = 2;
int const EncB = 3;

volatile byte pos;
volatile int  enc_count;

unsigned long s_time = 0;
unsigned long f_time = 0;
unsigned long m_time = 0;
unsigned long s_timex = 0;
unsigned long f_timex = 0;

float ave = 0;
float diameter = 0;
float sum;   
int val = 0;
int cnt = 0;
bool flag = true;
bool flagb = true;
bool flagc = true;
bool flagx = true;
bool flagr = true;

float up = 0;
float corected_up = 0;
float height = 0;             //先端位置を保存する変数（補正前）
float corected_height = 0;    //先端位置を保存する変数（補正後）
float before_height;
float delta_height;
float keisoku = 0;
float Ans = 0;
float yes_date = 0;
float yes_height = 0;
float day = 0;

//カウント用変数
int n = 1;
int m;
int f = 0;
int var = 0;
int ms = 0;
bool tomeru;
//時間計測用変数
unsigned long time;
unsigned long beforetime = 0;


void setup() {
  //2,3番ピンをINPUT専用に設定し、内部のプルアップ抵抗を有効にする
  pinMode(EncA,INPUT);
  pinMode(EncB,INPUT);

  //モーター用ピン設定
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  //スイッチ用ピン
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  //LED用ピン
  pinMode(13, OUTPUT);

  Serial.begin(9600);
  servo1.attach(5);
  servo1.write(0);
  delay(1500);
  sum = 0;
}

void loop() {
  n = digitalRead(12);

  switch (n) {
    case 1:
    STOP_MOTOR();
    
      break;
    case 0:
      time = micros();
      if (time - beforetime >= 50000 ) {
        if (digitalRead(7) == HIGH && flag == true) {
          DOWN_MOTOR(255);
          ENC_READ();
//          Serial.println(Ans);
        } else if(digitalRead(7) == LOW | flagb == false){
          STOP_MOTOR();
          keisoku = Ans;
          flagb = false;
          while(var < 3){
            digitalWrite(13, HIGH);
            val = analogRead(A0);
            if(val < 30 && flag == true){
              flag = true;
              servo1.write(140);
              }else if (val > 100 && flag == true){
                s_time = millis();
                flag = false;
                servo1.write(140);
              }else if (val < 30 && flag == false){
                f_time = millis();
                m_time = f_time - s_time;
                diameter = (90.79 * m_time)/1000;
                //Serial.println(diameter);
                var = var + 1;
                sum = diameter + sum;
                servo1.write(0);
                cnt = cnt + 1;
                if(cnt == 3){
                  delay(1000);
                  day = day + 1;
                  Serial.print("day");
                  Serial.println(day);
                  Serial.print("stem average is");
                  Serial.println(sum/3);
                  delay(2000);
                  sum = 0;
                  cnt = 0;
                  m_time = 0;
                  f_time = 0;
                  s_time = 0;
                }
                delay(1500);
                flag = true;
                flagb = true;
                digitalWrite(13, LOW);
              }
          }
          delay(2000);
          delta_height = keisoku - yes_height;
          if(day == 1){
            yes_height = keisoku;
            Serial.print("Position");
            Serial.println("基準点を設定");
            delta_height = 0;
            f = 0;
            UP_MOTOR(128);
            ENC_READ();
            Ans = yes_date;
//            Serial.println(yes_date);
            delay(10000);
            f = 0;
            STOP_MOTOR;
//            Serial.println(Ans);
          }else if (day > 1){
            corected_height = corected_height + delta_height;
            yes_height = keisoku;
            Serial.println(yes_height);
            Serial.print("Position");
            Serial.println(corected_height);
            Serial.print("d_height");
            Serial.println(delta_height);
            delta_height = 0;
            keisoku = 0;
            f = 0;
            UP_MOTOR(128);
            ENC_READ();
            delay(10000);
            Ans = yes_date;
            f = 0;
            STOP_MOTOR;
//            Serial.println(Ans);
          }
          var = 0;
          flagb = true;
          f = 0;
          n = 0;
          beforetime = millis();
        }     
      } else {}
      break;
  }
}

void UP_MOTOR(int s) {
  digitalWrite(9, 0);
  digitalWrite(10, s);
}

void DOWN_MOTOR(int s) {
  digitalWrite(9, s);
  digitalWrite(10, 0);
}

void STOP_MOTOR() {
  digitalWrite(9, 0);
  digitalWrite(10, 0);
}

void ENC_READ() {
  valEncA = digitalRead(EncA);
  valEncB = digitalRead(EncB);
  if (valEncA == 0 && valEncB == 1 && flagx == true){
    f = f + 1;
    flagx = false;
  }else if (valEncA == 1 && valEncB == 0 && flagx == true){
    f = f - 1;
    flagx = false; 
  }else if (valEncA == 1 && valEncB == 1){
    flagx = true;
  }
  if (millis() > f_timex ){
    s_timex = millis();
    f_timex = s_timex + 500;
    Ans = f * 1.51;
//    Serial.print("movement is ");
//    Serial.println(Ans); 
  }
}
