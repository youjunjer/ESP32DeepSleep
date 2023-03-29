#include "driver/rtc_io.h" //RTC腳位上拉下拉設定

//使用RTC記憶體紀錄喚醒次數
RTC_DATA_ATTR int RTCBootCount = 0;
//使用一般記憶體紀錄喚醒次數
int MemeryBootCount = 0;

void setup(){
  Serial.begin(115200);

  //設定腳位喚醒
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, HIGH); //當IO33 HIGH時喚醒
  rtc_gpio_pulldown_en(GPIO_NUM_33); //將33下拉
  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, 0); //當IO32 LOW時喚醒
  //esp_sleep_enable_ext0_wakeup(GPIO_NUM_25, 0); //當IO25 LOW時喚醒

  //印出喚醒原因
  print_wakeup_reason();

  //印出喚醒次數
  Serial.println("RTCBootCount : " + String(RTCBootCount++));

  //在GPIO 13接一個LED燈，當作喚醒指示燈
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  delay(10000);


  Serial.println("進入休眠");
  esp_deep_sleep_start();
  Serial.println("setup:深度睡眠以後的程式內容都不會被執行");
}

void loop(){
  Serial.println("loop:深度睡眠以後的程式內容都不會被執行");
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



