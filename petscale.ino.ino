/*
  RFID SheldとESP8266、7Segduinoを組み合わせたペット用体重計
  RFIDチップで個体管理しnifty MobileBackendにデータを書き込む。
  RFIDチップを読ませてからHX711モジュールで計測し、計測結果を７Segduinoへリアルタイムで送信。
  計測確定後ESP8266からniftyMobileBackendへ送信する。

  次のバージョン以降での改定予定
  UXの改良
    モードごとに鳴動してユーザに知らせる。(完了）
    Wifiの状態を光と鳴動で知らせる。（完了）
  トイレモード
    計測後の差分を保存する
    ※個体管理なし
  WifiManager対応（完了）
    wifi設定を変更可能にする（WifiManagerではなくSPIFFSを使用して独自に作成）
  動作順序の切り替え
    計測後にRFIDを読ませるモード追加
    ※RFIDの読み取りがなければ計測のみ
    ※スイッチで切り替え
    計測揺れを強制的に平均を算出する
    ※スイッチで過去3回分の平均値を送信
*/ 
#include "HX711.h"
//---- RFID
#include <SoftwareSerial.h>
float gravityval;
float dispgrav;

//#define calibration_factor 220462 //Beam Type Load cellの場合
//#define zero_factor 215244 //Beam Type Load cellの場合

#define calibration_factor -47260 // SparkFun_HX711_Calibration sketch で算出
#define zero_factor -412340 // SparkFun_HX711_Calibration sketchで算出した参考値

#define DOUT  A1
#define CLK  A0
#define SOFTRX 6
#define SOFTTX 7

#define SOFT2RX 8
#define SOFT2TX 9
#define SOFT3RX 10
#define SOFT3TX 11
#define RSTSW 5
#define TONESW 12
#define BUF_SIZE 32
#define BEAT 50

HX711 scale(DOUT, CLK);
int timecount=0;
int bval,nval=0;
int CloudSended=0;

int RunningMode=0;
//---- RFID 
SoftwareSerial RFIDSoftSerial(SOFTRX, SOFTTX);   // To RFID
SoftwareSerial ESPSoftSer(SOFT2RX, SOFT2TX); // To ESP8266
SoftwareSerial SevenSegSoftSer(SOFT3RX, SOFT3TX); // To 7Segduino
unsigned char buffer[BUF_SIZE];       // buffer array for data receive over serial port
int count = 0;                    // counter for buffer array
unsigned char Cnumber[BUF_SIZE];
unsigned long CNumL=0L;
int Ccount = 0;                   // counter for CardNumber Global Buffer
int Reading = 0;

void setup()
{
    RFIDSoftSerial.begin(9600);     // RFID リーダー用
    ESPSoftSer.begin(9600);         // ESP8266 通信用
    SevenSegSoftSer.begin(4800);    // ７Segduino通信用
    Serial.begin(4800);             // debug用
    // カード番号用初期化
    Serial.println("Setup!");
    CNumL=0L;
    Cnumber[0] = buffer[0] = 0x0;

    pinMode(RSTSW, INPUT); // RESET BUTTON
    delay(500);
  
    RunningMode = 1; // RFID ReadMode で起動
    //RunningMode = 2; // test debug
    CloudSended = 0; // クラウドにデータ未送信
    ClickBeep(0);    // 初期化音
}
 
void loop()
{
  if(RunningMode == 1){
    RFID_ReadLoop();    // RFID読み取りモード
  }else if(RunningMode == 0){
    HX711_Loop();       // 重量計測モード
  }else{
    if(digitalRead(RSTSW) == HIGH){                // Sound test Mode
      tone(TONESW,840,BEAT);
      delay(BEAT);
      tone(TONESW,1400,BEAT);
      delay(1000);
    }
  }
}

void RFID_ReadLoop() // RFIDデータは非同期のためデータが揃うまでループ
{
  RFIDSoftSerial.listen(); RFID用SoftSerialの準備
  //--------------RFID Reader からデータが来た
  if (RFIDSoftSerial.available())              
  {
      while(RFIDSoftSerial.available())                 // シリアルバッファがある間ループ
      {
          buffer[count++] = RFIDSoftSerial.read();      // １文字ずつバッファ
          if(count == BUF_SIZE)break;
      }
      for(int i=0;i < count; i++){
        if((buffer[i] >= 0x30 && buffer[i] <= 0x39) || (buffer[i] >= 0x41 && buffer[i] <= 0x46)){
          Cnumber[Ccount++] = buffer[i];                // 0-9 A-F
        }
      }
      clearBufferArray();             // 受取バッファのクリア
      count = 0;
  }else{
    if(Ccount > 11){                  // 11文字を上限として番号を整形
      Cnumber[Ccount] = 0x0;
      int cnt=0;
      buffer[cnt++] = '0'; // convert buffer(hex) to unsigned long 
      buffer[cnt++] = 'x'; // RFIDはhexで読み取られるためunsignedLongに変換するとタグに記載の番号となる
      for(int i=2; i<10; i++){
        buffer[cnt++] = Cnumber[i];
      }
      buffer[cnt] = 0x0;
      CNumL = strtoul(buffer,NULL,16);  // unsigned long で保存しておく
      ClickBeep(2); // このタイミングで音を鳴らしてRFID読み取り成功を知らせる
      Serial.print("cardNumber:");
      Serial.println(CNumL); // １０進数に変換したものを表示
      clearBufferCArray(); // buffer clear
      Ccount=0;
      RunningMode = 0; // Calc Mode
      //-----------HX711 Setup　計測直前にセットアップしないとスリープしたのでここでセットアップ    
      scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
      scale.set_offset(scale.read_average()); //Zero out the scale using a previously known zero_factor
      timecount = 0;
      Serial.println("ScaleSetup.");
      ClickBeep(1); // ここで音を鳴らして計測可能になったことを知らせる。
      return;
      //-----------HX711 Setup
    }
  }
}

void HX711_Loop()
{
  String espWriteBuf;

  if(digitalRead(RSTSW) == HIGH){ // ここでリセットすると風袋引き
      scale.set_offset(scale.read_average());
  }
  if(timecount >= 2){ // 10SPSモードにしているので約2秒間静止出来たら計測終了
    if(digitalRead(RSTSW) == HIGH){ // リセットボタンを実装
      timecount=0;
      setup();
      return;
    }
    if(CloudSended){  // 送信後はリセットされるまで待機
      return;
    }else{
      // nifty Mobile backend へ送信
      // uid,weihgt の形式でシリアル送信すればクラウドへ送信する。
      ESPSoftSer.listen();
      CloudSended=1;
      espWriteBuf=String(CNumL,DEC)+","+String(gravityval); // 送信用整形
      ESPSoftSer.println(espWriteBuf);
      Serial.println("CloudSend!");
      Serial.println(espWriteBuf);
      ClickBeep(3); // ここで音を鳴らして送信完了を知らせる
      return; // Send Value to Cloud.    
    }
  }
  // 5回平均値を使用しているがシリアルの速度が稼げれば10回でも良い
  // 10SPSモードなのでtimecount=1sec
  gravityval = (scale.get_units(10)*2146.66666666666667f)/1000.00f; // 1g = 0.00220462 lbs
  nval = (int)(gravityval*100.0f);  // 静止比較用
  SevenSegSoftSer.println(gravityval); // 7Segduinoへ書き込み
  Serial.println(gravityval);   // debug print
  if(nval!=0 && bval == nval){
    timecount++;    // 重量が安定したらカウントアップ
  }else{
    timecount = 0;
  }
  bval = nval;
  //delay(1);
}

void clearBufferArray()                 // function to clear buffer array
{
    // clear all index of array with command NULL
    for (int i=0; i<BUF_SIZE; i++)
    {
        buffer[i]=0x0;
    }                  
}
void clearBufferCArray()                 // function to clear CardNumber buffer array
{
    // clear all index of array with command NULL
    for (int i=0; i<BUF_SIZE; i++)
    {
        Cnumber[i]=0x0;
    }                  
}

void ClickBeep(int mode)  // モードごとに音を変える
{
  switch(mode){
    case 1:   // CALC
      tone(TONESW,840,BEAT);
      delay(BEAT);
      tone(TONESW,1400,BEAT);
      delay(500);
      break;
    case 2:   // RFID
      tone(TONESW,1500,BEAT);
      delay(BEAT);
      tone(TONESW,1500,BEAT);
      delay(500);
      break;
    case 3:   // END
      tone(TONESW,840,300);
      delay(300);
      tone(TONESW,1000,300);
      delay(300);
      tone(TONESW,1400,300);
      delay(300);
      break;
    default:  // Error
      tone(TONESW,940,700);
      delay(500);
      break;
  }
}

