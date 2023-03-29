#define Threshold 30 //觸控程度
//未觸摸大約在80~100，觸摸後在30以下

//使用RTC記憶體紀錄喚醒次數
RTC_DATA_ATTR int RTCBootCount = 0;
//使用一般記憶體紀錄喚醒次數
int MemeryBootCount = 0;

touch_pad_t touchPin=esp_sleep_get_touchpad_wakeup_status();//宣告觸摸腳位

//顯示觸控喚醒腳位
void print_wakeup_touchpad() {
  Serial.print("腳位觸摸：");
  switch (touchPin) {
    case 0  : Serial.println("GPIO 4"); break;
    case 1  : Serial.println("GPIO 0"); break;
    case 2  : Serial.println("GPIO 2"); break;
    case 3  : Serial.println("GPIO 15"); break;
    case 4  : Serial.println("GPIO 13"); break;
    case 5  : Serial.println("GPIO 12"); break;
    case 6  : Serial.println("GPIO 14"); break;
    case 7  : Serial.println("GPIO 27"); break;
    case 8  : Serial.println("GPIO 33"); break;
    case 9  : Serial.println("GPIO 32"); break;
    default : Serial.println("非觸控喚醒"); break;
  }
}

void TouchCallback() {
  //Do nothing
}

void setup() {
  Serial.begin(115200);

  //設定T8(GPIO33)觸控中斷
  touchAttachInterrupt(T8, TouchCallback, Threshold);

  //設定T9(GPIO32)觸控中斷
  touchAttachInterrupt(T9, TouchCallback, Threshold);

  //設定觸控啟動
  esp_sleep_enable_touchpad_wakeup();
  
  //印出喚醒原因
  print_wakeup_reason();
  print_wakeup_touchpad();

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

void loop()
{
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


