/*
 PetScale V0.7
  RFID Sheld��ESP8266�A7Segduino��g�ݍ��킹���y�b�g�p�̏d�v
  RFID�`�b�v�Ō̊Ǘ���nifty MobileBackend�Ƀf�[�^���������ށB
  RFID�`�b�v��ǂ܂��Ă���HX711���W���[���Ōv�����A�v�����ʂ��VSegduino�փ��A���^�C���ő��M�B
  �v���m���ESP8266����niftyMobileBackend�֑��M����B


 ���̃o�[�W�����ȍ~�ł̉���\��
  UX�̉���
    ���[�h���Ƃɖ����ă��[�U�ɒm�点��B(�����j
    Wifi�̏�Ԃ����Ɩ��Œm�点��B�i�����j
  �g�C�����[�h
    �v����̍�����ۑ�����
    ���̊Ǘ��Ȃ�
  WifiManager�Ή��i�����j
    wifi�ݒ��ύX�\�ɂ���iWifiManager�ł͂Ȃ�SPIFFS���g�p���ēƎ��ɍ쐬�j
  ���쏇���̐؂�ւ�
    �v�����RFID��ǂ܂��郂�[�h�ǉ�
    ��RFID�̓ǂݎ�肪�Ȃ���Όv���̂�
    ���X�C�b�`�Ő؂�ւ�
    �v���h��������I�ɕ��ς��Z�o����
    ���X�C�b�`�ŉߋ�3�񕪂̕��ϒl�𑗐M
*/ 
#include "HX711.h"
//---- RFID
#include <SoftwareSerial.h>
float gravityval;
float dispgrav;


/#define calibration_factor 220462 //Beam Type Load cell�̏ꍇ
//#define zero_factor 215244 //Beam Type Load cell�̏ꍇ


define calibration_factor -47260 // SparkFun_HX711_Calibration sketch �ŎZ�o
#define zero_factor -412340 // SparkFun_HX711_Calibration sketch�ŎZ�o�����Q�l�l


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
    RFIDSoftSerial.begin(9600);     // RFID ���[�_�[�p
    ESPSoftSer.begin(9600);         // ESP8266 �ʐM�p
    SevenSegSoftSer.begin(4800);    // �VSegduino�ʐM�p
    Serial.begin(4800);             // debug�p
    // �J�[�h�ԍ��p������
    Serial.println("Setup!");
    CNumL=0L;
    Cnumber[0] = buffer[0] = 0x0;


    pinMode(RSTSW, INPUT); // RESET BUTTON
    delay(500);
  
    RunningMode = 1; // RFID ReadMode �ŋN��
    //RunningMode = 2; // test debug
    CloudSended = 0; // �N���E�h�Ƀf�[�^�����M
    ClickBeep(0);    // ��������
}

void loop()
{
  if(RunningMode == 1){
    RFID_ReadLoop();    // RFID�ǂݎ�胂�[�h
  }else if(RunningMode == 0){
    HX711_Loop();       // �d�ʌv�����[�h
  }else{
    if(digitalRead(RSTSW) == HIGH){                // Sound test Mode
      tone(TONESW,840,BEAT);
      delay(BEAT);
      tone(TONESW,1400,BEAT);
      delay(1000);
    }
  }
}

void RFID_ReadLoop() // RFID�f�[�^�͔񓯊��̂��߃f�[�^�������܂Ń��[�v
{
  RFIDSoftSerial.listen(); RFID�pSoftSerial�̏���
  //--------------RFID Reader ����f�[�^������
  if (RFIDSoftSerial.available())              
  {
      while(RFIDSoftSerial.available())                 // �V���A���o�b�t�@������ԃ��[�v
      {
          buffer[count++] = RFIDSoftSerial.read();      // �P�������o�b�t�@
          if(count == BUF_SIZE)break;
      }
      for(int i=0;i < count; i++){
        if((buffer[i] >= 0x30 && buffer[i] <= 0x39) || (buffer[i] >= 0x41 && buffer[i] <= 0x46)){
          Cnumber[Ccount++] = buffer[i];                // 0-9 A-F
        }
      }
      clearBufferArray();             // ���o�b�t�@�̃N���A
      count = 0;
  }else{
    if(Ccount > 11){                  // 11����������Ƃ��Ĕԍ��𐮌`
      Cnumber[Ccount] = 0x0;
      int cnt=0;
      buffer[cnt++] = '0'; // convert buffer(hex) to unsigned long 
      buffer[cnt++] = 'x'; // RFID��hex�œǂݎ���邽��unsignedLong�ɕϊ�����ƃ^�O�ɋL�ڂ̔ԍ��ƂȂ�
      for(int i=2; i<10; i++){
        buffer[cnt++] = Cnumber[i];
      }
      buffer[cnt] = 0x0;
      CNumL = strtoul(buffer,NULL,16);  // unsigned long �ŕۑ����Ă���
      ClickBeep(2); // ���̃^�C�~���O�ŉ���炵��RFID�ǂݎ�萬����m�点��
      Serial.print("cardNumber:");
      Serial.println(CNumL); // �P�O�i���ɕϊ��������̂��f�o�b�O�\��
      clearBufferCArray(); // �J�[�h�i���o�[�o�b�t�@�̃N���A
      Ccount=0;
      RunningMode = 0; // Calc Mode
      //-----------HX711 Setup�@�v�����O�ɃZ�b�g�A�b�v���Ȃ��ƃX���[�v�����̂ł����ŃZ�b�g�A�b�v    
      delay(400);
      scale.set_scale(calibration_factor); // Calibration_factor��SparkFun_HX711_Calibration �X�P�b�`�ŎZ�o���Ă���
      delay(400);
      scale.set_offset(scale.read_average()); // �������܈���
      delay(500);
      timecount = 0;
      Serial.println("ScaleSetup.");
      ClickBeep(1); // �����ŉ���炵�Čv���\�ɂȂ������Ƃ�m�点��B
      return;
      //-----------HX711 Setup
    }
  }
}

void HX711_Loop()
{
  String espWriteBuf;

  if(timecount >= 2){ // 10SPS���[�h�ɂ��Ă���̂Ŗ�2�b�ԐÎ~�o������v���I��
    if(digitalRead(RSTSW) == HIGH){ // ���Z�b�g�{�^��������
      timecount=0;
      setup();
      return;
    }
    if(CloudSended){  // ���M��̓��Z�b�g�����܂őҋ@
      return;
    }else{
      // nifty Mobile backend �֑��M
      // uid,weihgt �̌`���ŃV���A�����M����΃N���E�h�֑��M����B
      ESPSoftSer.listen();
      CloudSended=1;
      espWriteBuf=String(CNumL,DEC)+","+String(gravityval); // ���M�p���`
      ESPSoftSer.println(espWriteBuf);
      Serial.println("CloudSend!");
      Serial.println(espWriteBuf);
      ClickBeep(3); // �����ŉ���炵�đ��M������m�点��
      return; // Send Value to Cloud.    
    }
  }
  // 5�񕽋ϒl���g�p���Ă��邪�V���A���̑��x���҂����10��ł��ǂ�
  // 10SPS���[�h�Ȃ̂�timecount=1sec
  gravityval = (scale.get_units(10)*2146.66666666666667f)/1000.00f; // 1g = 0.00220462 lbs
  nval = (int)(gravityval*100.0f);  // �Î~��r�p
  SevenSegSoftSer.println(gravityval); // 7Segduino�֏�������
  Serial.println(gravityval);   // debug print
  if(nval!=0 && bval == nval){
    timecount++;    // �d�ʂ����肵����J�E���g�A�b�v
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

void ClickBeep(int mode)  // ���[�h���Ƃɉ���ς���
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
