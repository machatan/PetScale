# PetScale
Arduino を利用したペット体重計  

## Overview
ArduinoとGrove RFID SheldとESP8266、7Segduinoを組み合わせたペット用体重計です。  
RFIDチップで個体管理しnifty MobileBackendにデータを書き込む仕様です。  
RFIDチップを読ませてからHX711モジュールで計測し、計測結果を７Segduinoへリアルタイムで送信し体重を表示します。  
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

クラウドへの保存はnifty mobile backendを使用するのでアカウントを作成しておく  

http://mb.cloud.nifty.com/  
