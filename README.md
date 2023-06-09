# ESP32DeepSleep
ESP32深度睡眠實驗，
準備材料：

  1. ESP32
  2. 0.96 OLED  
  3. DHT11溫濕度計
  4. 杜邦線、麵包版等

教材講義：https://twgo.io/gxgmd

直播教學：https://www.youtube.com/watch?v=8w6kMkSfkvY

# 1.輕度睡眠與深度睡眠
讓ESP32沒工作時進入睡眠是最節省電力的最法，非常適合在戶外或者使用電池時開啟睡眠模式，本部分介紹如何進入睡眠？如何甦醒嗎？
目前ESP32有五種睡眠模式

![image](https://user-images.githubusercontent.com/40359899/228641276-c19dd35b-074f-42d0-98ce-81ff690e424a.png)

1.Active正常模式
就是我們一般的運作狀態，可以開啟藍芽及WiFi

2.Modem-Sleep通訊關閉
就是指沒有啟動藍芽或WiFi的狀態，這個狀態並不是一個完整定義的工作模式，只是我們手動關閉通訊模組時的狀態，所以並沒有在ESP32的規格內

3.Light-Sleep輕度睡眠
是指主CPU、記憶體呈現”暫停”狀態，當然通訊部份也是關閉狀態，這樣已經可以大幅達到省電的功能。

4.Deep-Sleep深度睡眠
是指主CPU、記憶體呈現”關閉”狀態，只剩下RTC週邊及RTC記憶體是開啟的，此時系統會比輕度睡眠更加省電。

5.Hibernation休眠
是指主CPU、記憶體、RTC週邊、RTC記憶體也全部都關閉，此時必須使用ULP協同CPU來做喚醒。

以下把比較常用的輕度及深度睡眠不同的部分再重新整理成一張表

![image](https://user-images.githubusercontent.com/40359899/228641454-e65d88b0-9362-443b-a371-a8f655de126e.png)

CPU：當輕度睡眠時，CPU為“暫停”，而深度睡眠時CPU則是完全“關閉”，暫停與關閉的差異就是CPU在睡眠前的狀態是否還存在，“暫停”代表未來喚醒後程式還可以「接著往下執行」，而”關閉”則是代表CPU完全斷電了，執行狀態、暫存器等資訊都已經遺失，因此”關閉”後未來喚醒後程式必須「從頭開始執行」。

主記憶體：如同CPU一樣，輕度睡眠時記憶體內的變數會被”保留”，未來喚醒後仍可取用這些變數的值，但是深度睡眠時主記憶體內容會被”清除”，代表未來喚醒後，是無法存取睡眠前的任何變數，此時必須使用RTC記憶體來記錄變數。

程式流程：當輕度睡眠時，喚醒後的程式會“接續”執行睡眠後的指令，而深度睡眠喚醒後，由於CPU及記憶體都已重設，因此必須”從頭”開始執行，因此睡眠後的任何指令都不會被執行到，此時若有需要存放變數則必須使用RTC記憶體。

腳位狀態：輕度睡眠時，腳位的狀態會被”保留”，例如某腳位設定為HIGH，則輕度睡眠時依然為HIGH，但深度睡眠時，所有腳位的狀態都被”清除”，並不會記得睡眠前的腳位狀態。

![image](https://user-images.githubusercontent.com/40359899/228641558-e1c135cf-32a2-4ab0-80a8-32f84e7c4a6f.png)

# 2.喚醒方式
由於睡眠時CPU進入暫停或關閉狀態，無法進行感測，此時必須藉由內部RTC來處理，主要喚醒方式可以分成以下五種：

1.TIMER：由RTC計時器喚醒
TIMER最為簡單，我們利用「esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP)」就可以設定未來進入睡眠後，應該多久後喚醒，有點像是人類在睡覺前會定鬧鐘一樣，這裡的”TIME_TO_SLEEP”時間是以百萬之一秒，所以有時候我們會把時間x1000x1000，轉換成秒。

輕度睡眠定時喚醒範例：01LightSleep.ino

輕度睡眠定時喚醒並讀取DHT11傳送MQTT範例：02LightSleepDHT11MQTT.ino

深度睡眠定時喚醒範例：03DeepSleep.ino

深度睡眠定時喚醒並讀取DHT11傳送MQTT範例：04DeepSleepDHT11MQTT.ino


2.EX0：RTC 單一腳位喚醒
利用RTC來感測某特定腳位是HIGH或LOW時來喚醒，例如：esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, HIGH)，代表當GPIO33為HIGH時就喚醒，此處不能寫數字33，而必須使用”GPIO_NUM_33″，因為這個是要給RTC看的，RTC的腳位並非全部都可以，請參閱下表，有標示的才可以拿來喚醒，另外測試時建議搭配rtc_gpio_pulldown_en(GPIO_NUM_33);先將腳位拉低。

這裡有一個地方要注意，這個指令為單一腳位，因此若你同時寫很多行來設定多隻腳喚醒時，只有最後一個設定能作用，例如：

esp_sleep_enable_ext0_wakeup(GPIO_NUM_32, HIGH);

esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, HIGH);

esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, HIGH);

這裡設定33,32,2號腳可以喚醒，不過最後只有2號腳可以喚醒系統。若需要使用多腳位則要下面EX1的功能。

深度睡眠單腳位喚醒範例：05DeepSleepWakeByPin.ino

3.EX1：RTC 多腳位喚醒
當使用者有需求喚醒腳位超過一隻時，就必須使用這個功能，由於很多腳位要用一個BITMASK變數來代替，因此必須了解如何計算BITMASK，這裡先假設ESP32只有10根腳(編號0~9)，而我們希望9,6,2能夠喚醒。

![image](https://user-images.githubusercontent.com/40359899/228641744-9e93cbf0-3fda-4f09-a90f-ba3916d9b615.png)

在觸發腳9,6,2腳位編號下面寫上1，其餘不會觸發喚醒的腳位寫0，那麼這個2進位的數字就是「1001000100」，換成10進位=2^9+2^6+2^2=580

再換成16進位為0x244，那麼BITMASK= 0x244了。

相同的如果這次我們要用ESP32的33,32,2三隻腳來喚醒，則計算2^33+2^32+2^2=10進位12884901892 =16進位0x300000004

進位轉16進位可以使用以下這個網站：https://www.convertworld.com/zh-hant/numerals/hexadecimal.html

深度睡眠多腳位喚醒範例：06DeepSleepWakeByMultiPins.ino


4.TOUCHPAD：RTC 觸控腳喚醒
ESP32內建10個觸控腳，可以感測人體靜電，當有人觸摸時，數值會降到30以下，因此我們可以設定觸控門檻值為30，

touchAttachInterrupt(T8, TouchCallback, Threshold);代表觸控腳T8被觸摸時，系統就會被喚醒。詳細操做語法請參考

深度睡眠觸控喚醒範例：07DeepSleepWakeByTouch.ino


5.ULP：協同CPU喚醒(本篇不實做)
這裡有幾點要注意，ESP32當進入睡眠狀態，雖然腳位也關閉了，但VCC及GND還是保持送電狀態，若希望感測器在休眠時，也關閉感測器，可以將感測器的VCC腳利用某個IO送HIGH來取代，例如DHT11的VCC接在IO 32，然後IO32送HIGH，這樣DHT11在休眠時會關機，而喚醒後也可以感測溫濕度。

另外一個是建議電力不要從ESP32的USB接入，因為當ESP32休眠時，模組上面的USBTTL並沒有一起休眠(例如CH340或CP210x)，而這也會耗用電力的，如果希望電力能撐更久的話，就避免接到USB，而是接到ESP32的VIN、GND腳，或者直接使用有付電池插孔的ESP32。
