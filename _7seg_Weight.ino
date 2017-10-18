/*
From Serial to NumberDisplay
シリアル通信で計測した数値を整数2桁、小数点以下2桁の計4桁で受け取る。
入力は文字列でXX.XXの形式である。
整数部、小数部ともに3桁以上あるとエラー表示となる。
ただし、小数部は省略できる。
通信ボーレートは4800bpsよりも上げないほうが良い。
*/
 
#include <Sseg.h>
  
Sseg mySseg = Sseg(4, 8, 11, 13, 14, 2, 10, 12, 3, 6, 7, 9);
 
long time = 8888;
int LEDflag=0;
unsigned long count=0;
unsigned long nowt=0;
boolean errFlg=false;
 
void setup() {
 Serial.begin(4800);
 
 mySseg.setZeroSupress(0);
 mySseg.begin();
 mySseg.turnOff();
}
 
void parseLine(char *s) {
 char *p;
 long lsb;
 bool one;
 
  //Serial.println(s);
  p = strtok(s, "."); //小数点までを読み込む
  time=atol(p)*100;
  //if(time>10000){
  //  time=9900;
  //}
  //Serial.println(time);
  while ( p != NULL ) {
          p = strtok( NULL,"." ); //小数点以下を読み込む
          if ( p != NULL ){
            if(strlen(p) == 1){ // 小数点以下が一桁の場合100gとみなす
              one = true;
            }else{
              one = false;
            }
            lsb=atol(p);
            if (lsb >= 100){
              time=10000;
            }else{
              if(one) lsb*=10;
              time+=lsb;
            }
          }
  }
  //Serial.println("last");
  //Serial.println(time);
}
 
void loop() {
 static char buf[256];
 static int ptr = 0;
 
 while (Serial.available() > 0) {
 if (((buf[ptr] = Serial.read()) == '\n') ||
 ((buf[ptr] == '\r'))) { // CR LFのみ扱うためLFは読み飛ばし
  if(buf[ptr] != '\r'){
      buf[ptr] = 0;
      parseLine(buf); //表示文字列のパース
      ptr = 0;
      LEDflag=1;
      count=millis(); //基準時間の取得
   //Serial.println(millis());
    }
  } else {
    ptr++;
  }
 }

 if(LEDflag){ //LED表示
   mySseg.turnOn();
   nowt=millis();
   if(time>=10000){ // Errorh表示
    mySseg.writeRawData(0x9e,0x0a,0x0a,0x10); // Err_ の点滅
    mySseg.updateWithDelay(500);
    mySseg.turnOff();
    delay(500);
    mySseg.turnOn();
    mySseg.updateWithDelay(500);
    mySseg.turnOff();
    delay(500);
    mySseg.turnOn();
    mySseg.updateWithDelay(500);
    mySseg.turnOff();
    count=0;
    LEDflag=0;
    errFlg=true;
    return;
   }else{
    mySseg.writeNum(time); //計測数値の表示
    mySseg.setDot(1); //小数点表示
    mySseg.updateWithDelay(4);
   }
   if(count+3000L < millis() && !errFlg){ //３秒間表示
    count=0;
    nowt=0;
    LEDflag=0;
    mySseg.turnOff();  // 3秒後に確定点滅、その後10秒間表示
    delay(500);
    mySseg.turnOn();
    mySseg.updateWithDelay(500);
    mySseg.turnOff();
    delay(500);
    mySseg.turnOn();
    mySseg.updateWithDelay(500);
    mySseg.turnOff();
    delay(500);
    mySseg.turnOn();
    mySseg.updateWithDelay(10000);
    return;
   }else{
    errFlg=false;
    LEDflag=1;
   }
 }else{
  mySseg.turnOff();
 }
}
