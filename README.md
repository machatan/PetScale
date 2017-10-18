# PetScale
===
Arduino を利用したペット体重計  

## Overview
ArduinoとGrove RFID SheldとESP8266、7Segduinoを組み合わせたペット用体重計です。  
RFIDチップで個体管理しnifty MobileBackendにデータを書き込む仕様です。  
RFIDチップを読ませてからHX711モジュールで計測し、計測結果を７Segduinoへリアルタイムで送信し体重を表示します。  
計測確定後ESP8266からniftyMobileBackendへ送信します。※要アカウント  

## 利用するもの
測定＋RFID読取）  
Arduino Pro Mini　（5Vのものを使用）
https://www.switch-science.com/catalog/946/
Groveシリーズ用RFIDリーダー(Grove用4ピンコネクタ搭載)　単価 ¥1890  
http://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=EEHD-0GKE  
Seeed Studio　125KHz RFIDタグ 5個入り 単価 ¥370  
http://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=EEHD-0RBC  
人間用体重計（センサがばらしやすく単品でケースにまとまっているほうがいい）  
https://www.amazon.co.jp/1byone-%E3%83%87%E3%82%B8%E3%82%BF%E3%83%AB%E4%BD%93%E9%87%8D%E8%A8%88-%E3%83%9C%E3%83%87%E3%82%A3%E3%83%BC%E3%82%B9%E3%82%B1%E3%83%BC%E3%83%AB-%E8%A8%88%E9%87%8F%E7%AF%84%E5%9B%B2180kg%E3%81%BE%E3%81%A7-%E3%82%B7%E3%83%B3%E3%83%97%E3%83%AB%E3%81%AA%E3%83%87%E3%82%B6%E3%82%A4%E3%83%B3/dp/B06XSXLMJ8/ref=sr_1_3?ie=UTF8&qid=1508143214&sr=8-3&keywords=%E4%BD%93%E9%87%8D%E8%A8%88+%E3%82%AC%E3%83%A9%E3%82%B9  
SparkFun　SEN-13879 ロードセルアンプ HX711 単価 ¥1200  
http://www.sengoku.co.jp/mod/sgk_cart/detail.php?code=EEHD-4ZU4  
ロードセルコンビネーションボード  
https://www.sparkfun.com/products/13878  

測定結果表示）  
7segduinoキット  
https://www.switch-science.com/catalog/745/  

測定結果送信）  
ESPr® Developer（ESP-WROOM-02開発ボード）　単価 1,944円  
https://www.switch-science.com/catalog/2500/  
ロジック変換ボード  
https://www.switch-science.com/catalog/1523/  
低損失レギュレータ  
http://www.akizukidenshi.com/catalog/g/gI-00538/  

クラウドへの保存はnifty mobile backendを使用するのでアカウントを作成しておく  
http://mb.cloud.nifty.com/  
