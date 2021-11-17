#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

// use ArduinoJson by Benoit Blanchon
char ssid[] = "Family+88:6A:E3:FF:D6:EA"; //請修改為自己的SSID
char password[] = "2005mar14"; //請修改自己的密碼
char url[] = "http://opendata2.epa.gov.tw/AQI.json"; //PM2.5的網址
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.print("開始連線到無線網路SSID:");
  Serial.println(ssid);
  //1.設定WiFi模式
  WiFi.mode(WIFI_STA);
  //2.啟動WiFi連線
  WiFi.begin(ssid, password);
  //3.檢查連線狀態
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("連線完成");
}

void loop() {
    //4.啟動網頁連線
    Serial.print("啟動網頁連線");
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    Serial.print("httpCode=");
    Serial.println(httpCode);
    //5.檢查網頁連線是否正常
    if (httpCode == HTTP_CODE_OK) {
      //6.取得網頁內容
      String payload = http.getString();
      //Serial.print("payload=");
      //Serial.println(payload);
      DynamicJsonDocument AQJarray(payload.length() * 2);
      deserializeJson(AQJarray, payload); //解析payload為JSON Array格式
      String KLPM25 = AQJarray[0]["PM2.5"]; //第0個是基隆的PM2.5
      String XHPM25 = AQJarray[1]["PM2.5"]; //第1個是汐止的PM2.5
      Serial.println("基隆PM2.5：" + KLPM25);
      Serial.println("汐止PM2.5：" + XHPM25);
      for (int i = 0; i < AQJarray.size(); i++) {
        if (AQJarray[i]["SiteId"] == "48") {
          //橋頭站SiteId為"48"
          String QTPM25 = AQJarray[i]["PM2.5"];
          Serial.println("橋頭PM2.5：" + QTPM25);
          break; //退出for迴圈
        }
      }
    }  
    http.end(); //關閉網頁連線
    delay(10000);
}        
