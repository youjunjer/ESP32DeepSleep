#define TIME_TO_SLEEP  10        /* Time ESP32 will go to sleep (in seconds) */
//使用一般記憶體紀錄喚醒次數
int MemeryBootCount = 0;//等等將與深度睡眠做比較

void setup() {
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  //定義喚醒時間
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * 1000 * 1000);
}

void loop() {
  Serial.println("MemeryBootCount = " + String(MemeryBootCount++));
  digitalWrite(13,HIGH);
  Serial.println("Do Your Jobs");
  delay(10000);
  Serial.println("進入輕度睡眠");
  digitalWrite(13,LOW);
  esp_light_sleep_start();
  //印出喚醒原因
  print_wakeup_reason();
  //印出喚醒次數  
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



