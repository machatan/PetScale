/*
nifty mobile backend transmitter for ESP8266 via HTTPS
wifi manager include
 */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <String.h>

const char* host = "mb.api.cloud.nifty.com";
const int httpsPort = 443;

unsigned char inputStr[30];
int count=0;
String uwJson;
boolean parse_Suc;

WiFiClientSecure client;
// モード切り替えピン
const int MODE_PIN = 0; // GPIO0
// ステータスLED
const int LED_PIN = 13; // GPIO13

// Wi-Fi設定保存ファイル
const char* wifi_settings = "/wifi_settings.txt";
ESP8266WebServer server(80);

void handleRootGet() {
  String html = "";
  html += "<h1>WiFi Settings</h1>";
  html += "<form method='post'>";
  html += "  <input type='text' name='ssid' placeholder='ssid'><br>";
  html += "  <input type='text' name='pass' placeholder='pass'><br>";
  html += "  <input type='submit'><br>";
  html += "</form>";
  server.send(200, "text/html", html);
}

void handleRootPost() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  File f = SPIFFS.open(wifi_settings, "w");
  f.println(ssid);
  f.println(pass);
  f.close();

  String html = "";
  html += "<h1>WiFi Settings</h1>";
  html += ssid + "<br>";
  html += pass + "<br>";
  server.send(200, "text/html", html);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN,LOW);
}

void setup_client() {
//  File f = SPIFFS.open(wifi_settings, "r");
//  String ssid = f.readStringUntil('\n');
//  String pass = f.readStringUntil('\n');
//  f.close();
//
//  ssid.trim();
//  pass.trim();
//
//  Serial.println("SSID: " + ssid);
//  Serial.println("PASS: " + pass);
//
//  WiFi.begin(ssid.c_str(), pass.c_str());
  digitalWrite(LED_PIN,LOW);
//-------------------------SPIFSに書いた場合は、これだけで接続する情報もあるので成功したら上のcommentは消す
  WiFi.mode(WIFI_STA);
  WiFi.begin(); 
//-------------------------
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN,HIGH);
    delay(500);
    digitalWrite(LED_PIN,LOW);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    digitalWrite(LED_PIN,HIGH);
    delay(100);
    digitalWrite(LED_PIN,LOW);
    delay(100);
    digitalWrite(LED_PIN,HIGH);
    delay(100);
    digitalWrite(LED_PIN,LOW);
    delay(100);
    digitalWrite(LED_PIN,HIGH);
    delay(100);
    digitalWrite(LED_PIN,LOW);
    delay(100);
  digitalWrite(LED_PIN,HIGH);
  Serial.println("Ready");
}

void setup_server() {
  byte mac[6];
  WiFi.macAddress(mac);
  String ssid = "ESP8266_";
  for (int i = 0; i < 6; i++) {
    ssid += String(mac[i], HEX);
  }
  Serial.println("SSID: " + ssid);

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid.c_str());

  server.on("/", HTTP_GET, handleRootGet);
  server.on("/", HTTP_POST, handleRootPost);
  server.begin();
  Serial.println("Start SOFT AP Mode.");
}

void setup() {
  //Serial.begin(115200);
  Serial.begin(9600);
  // 2秒以内にMODEを切り替えるにはGPIO ０ボタンを押し続ける
  //  0 : Server
  //  1 : Client
  delay(2000);

  // ファイルシステム初期化
  SPIFFS.begin();

  pinMode(LED_PIN, OUTPUT);
  pinMode(MODE_PIN, INPUT);
  if (digitalRead(MODE_PIN) == 0) {
    // サーバモード初期化
    setup_server();
  } else {
    // クライアントモード初期化
    setup_client();
  }
}

void loop() {
  char c;
  String str;
  const char sendStr[32]="xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

  server.handleClient();
  
  if (Serial.available()) {
    //Serial.println("avail");
      c = Serial.read();
      if((c >= 0x30 && c <= 0x39) || (c == '.') || (c == ',')){// 数値と.,のみ受け付ける。
        inputStr[count] = c;
        count++;
      }else{
        inputStr[count] = '\x0';
      }
     //inputStr[count] = Serial.read();
     // 文字数が30以上 or 末尾文字
      if (count > 30 || c == '\n') {
        // 受信文字列を送信
        //Serial.println(count);
        //Serial.println((const char *)inputStr);
        parseJSON((char *)inputStr);
        if(parse_Suc==true){
            extServiceWrite();
        }
        Serial.println("Ready");
        // カウンタの初期化
        count = 0;
      }
  }
}

void parseJSON(char *str){ // input NumberStr XXXXXXXX,XX.XX
  char *uid;
  char *wgt;

  uid = strtok(str, ",");
  wgt = strtok(NULL,",");
  //Serial.println(uid);
  //Serial.println(wgt);
  if(uid==NULL || wgt==NULL){
    Serial.println("parseERROR!");
    parse_Suc=false;
  }else{
    uwJson="{\"uid\":\""+(String)uid+"\",\"weight\":\""+(String)wgt+"\"}";
    //Serial.println(uwJson);
    parse_Suc=true;
  }
}

void extServiceWrite(){
  String url = "/2013-09-01/classes/weight";
  // JSON ex. '{"uid":"12345678","weight":"3.67"}' in uwJson
  unsigned int fldLen;

 // Use WiFiClientSecure class to create TLS connection

  //WiFiClientSecure client;
  //Serial.print("connecting to ");
  //Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  fldLen = uwJson.length();
    
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: machaESP8266\r\n" +
               "Accept: */*\r\n" +
               "X-NCMB-Application-Key: 74de2ee04c4b87e54ab48dc8961f29a6da5382f9c9e8d04cb61e4822ffc17543\r\n" +
               "X-NCMB-Timestamp: 2017-09-23T12:00:12.442Z\r\n" +
               "X-NCMB-Signature: XEDk6qOt6PcA3MbiJP/YXp4m3iu7wlzgfBxFzRCnV+o=\r\n" +
               "Content-Type: application/json\r\n" +
               "Content-Length: "+String(fldLen,DEC)+"\r\n");
  client.println();
  client.println(uwJson);
  //Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    //Serial.println(line);
    if (line == "HTTP/1.1 201 Created\r") {
      Serial.println("headers received");
      digitalWrite(LED_PIN,LOW);
      delay(100);
      digitalWrite(LED_PIN,HIGH); // データ送信成功時に1秒間LED点灯
      delay(1000);
      digitalWrite(LED_PIN,LOW);
      delay(100);
      digitalWrite(LED_PIN,HIGH);
      break;
    }
  }
  String line = client.readStringUntil('\n');

  if (client.connected()) { 
    client.stop();  // DISCONNECT FROM THE SERVER
  }
}

