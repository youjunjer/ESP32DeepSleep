#include <WiFi.h>
#include <SimpleDHT.h>
#include <PubSubClient.h>
WiFiClient WifiClient;
PubSubClient MQTTClient(WifiClient);
int pinDHT11 = 4;
SimpleDHT11 dht11(pinDHT11);

char STA_SSID[] = "SSID";
char STA_PASS[] = "Password";

char* MQTTServer = "mqttgo.io";//免註冊MQTT伺服器
int MQTTPort = 1883;//MQTT Port
//推播主題1:推播溫度(記得改Topic)
char* MQTTPubTopic1 = "YourTopic/class205/temp";
//推播主題2:推播濕度(記得改Topic)
char* MQTTPubTopic2 = "YourTopic/class205/humi";

#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */
//使用一般記憶體紀錄喚醒次數
int MemeryBootCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  //定義喚醒時間
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000 * 1000);
}

void loop() {
  Serial.println("MemeryBootCount = " + String(MemeryBootCount++));
  digitalWrite(13, HIGH);
  //開啟網路
  enableWiFi();
  //連線MQTT
  MQTTConnecte();
  //讀取溫濕度
  byte temperature = 0;
  byte humidity = 0;
  ReadDHT(&temperature, &humidity);
  // ------ 將DHT11溫度送到MQTT主題 ------

MQTTClient.publish(MQTTPubTopic1, String(temperature).c_str());
MQTTClient.publish(MQTTPubTopic2, String(humidity).c_str());
  Serial.println("溫溼度已推播到MQTT Broker");
  delay(500);//等候上傳完畢

  Serial.println("工作完成，進入輕度睡眠");
  disableWiFi();
  digitalWrite(13, LOW);
  esp_light_sleep_start();
  //印出喚醒原因
  print_wakeup_reason();
  //印出喚醒次數
}

void disableWiFi() {
  WiFi.disconnect(true);  // Disconnect from the network
  WiFi.mode(WIFI_OFF);    // Switch WiFi off

  Serial.println("");
  Serial.println("網路關閉");
}

void enableWiFi() {
  Serial.println("開始連線");
  WiFi.begin(STA_SSID, STA_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected,IP address:"); Serial.println(WiFi.localIP());
}

//印出喚醒原因
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  Serial.println("-------------------------");
  Serial.print("喚醒原因：");
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("RTC腳位喚醒"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("RTC控制喚醒"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("定時喚醒"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("觸控喚醒"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("協同CPU喚醒"); break;
    default : Serial.printf("其他原因喚醒 %d\n", wakeup_reason); break;
  }
}

//開始MQTT連線
void MQTTConnecte() {
  MQTTClient.setServer(MQTTServer, MQTTPort);
  while (!MQTTClient.connected()) {
    //以亂數為ClietID
    String  MQTTClientid = "esp32-" + String(random(1000000, 9999999));
    if (MQTTClient.connect(MQTTClientid.c_str(), "", "")) {
      //連結成功，顯示「已連線」。
      Serial.println("MQTT已連線");
    } else {
      //若連線不成功，則顯示錯誤訊息，並重新連線
      Serial.print("MQTT連線失敗,狀態碼=");
      Serial.println(MQTTClient.state());
      Serial.println("五秒後重新連線");
      delay(5000);
    }
  }
}

//讀取DHT11溫濕度
void ReadDHT(byte *temperature, byte *humidity) {
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(temperature, humidity, NULL)) !=
      SimpleDHTErrSuccess) {
    Serial.print("讀取失敗,錯誤訊息=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    delay(1000);
    return;
  }
  Serial.print("DHT讀取成功：");
  Serial.print((int)*temperature);
  Serial.print(" *C, ");
  Serial.print((int)*humidity);
  Serial.println(" H");
}

