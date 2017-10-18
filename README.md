# PetScale
Arduino を利用したにゃんこ体重計   

![全体像](https://github.com/machatan/PetScale/blob/master/petscaleImages/IMG_9111.JPG) 

## Overview
ArduinoとGrove RFID SheldとESP8266、7Segduinoを組み合わせたにゃんこ用体重計です。  
RFIDチップで個体管理しnifty MobileBackendにデータを書き込む仕様です。  
RFIDチップを読ませてからHX711モジュールで計測し、計測結果を７Segduinoへリアルタイムで送信し体重を表示します。  
風袋引きがありますので台の上にあらかじめおやつやベッド、キャリーバッグを置いておくとにゃんこが安心します。  
計測確定後ESP8266からniftyMobileBackendへ送信します。※要アカウント  

## 利用するもの
### 測定＋RFID読み取り部  

Arduino Pro Mini（5Vのものを使用）  
https://www.switch-science.com/catalog/946/  

Groveシリーズ用RFIDリーダー(Grove用4ピンコネクタ搭載)  
http://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=EEHD-0GKE  

Seeed Studio　125KHz RFIDタグ  
http://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=EEHD-0RBC  

人間用体重計  

ロードセル4つが付いていてうまくケースに入っていればなんでもよい。

SparkFun　SEN-13879 ロードセルアンプ HX711  
http://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=EEHD-4ZU4  

ロードセルコンビネーションボード  
https://www.sparkfun.com/products/13878  

### 測定結果表示

7segduinoキット    
https://www.switch-science.com/catalog/745/  

### 測定結果送信  

ESPr® Developer（ESP-WROOM-02開発ボード）  
https://www.switch-science.com/catalog/2500/    
__シリアル通信のみなのでWio Nodeなどでもよいです__   

ロジック変換ボード   
https://www.switch-science.com/catalog/1523/  

低損失レギュレータ    
http://www.akizukidenshi.com/catalog/g/gI-00538/  

クラウドへの保存はnifty mobile backendを使用するのでアカウントを作成しておいてください。  

http://mb.cloud.nifty.com/    

## 利用方法
それぞれのスケッチをそれぞれのモジュールに転送したらArduino Pro MiniとRFID リーダー、7SegduinoとESP8266をそれぞれシリアル接続します。    
ESP8266が3.3VのためLDOで3.3Vを作って渡してやる必要があります。　　　

ESP8266モジュール用スケッチ：ESP8266ESP8266_HTTPSClient.ino　　　

７Segduino用スケッチ：_7seg_Weight.ino　　　

Arduino用スケッチ：petscale.ino　　　　

接続図　　　　
![接続図](https://github.com/machatan/PetScale/blob/master/petscaleImages/cabling.png)  　    

電源は電池ボックスなどにつないでください。  
ESP8266は上記接続前にWifi設定してください。  
電源を接続し、リセット後2秒以内にGPIO 0 を押しっぱなしにするとWifi設定モードに入りますので   
192.168.4.1 にブラウザで接続してSSIDとPWを設定してください。

## 使い方
１）電源を入れ、BEEP音が鳴ったらRFIDを読み取らせます。  
２）ピピッと音がして7SegduinoのLEDが点灯したらにゃんこを乗せて計測します。  
※暴れるようなら事前にベッドやバッグを乗せておきましょう  
３）約2秒間値が安定したら3回点滅し送信完了音が鳴ります。  
４）リセットボタンを押すとそのまま次のにゃんこを計れます。  
