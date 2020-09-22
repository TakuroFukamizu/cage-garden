//#include <M5Atom.h> // official library is not working with additional neopixels...
#include <FastLED.h>
#include <WiFi.h>
#include <DFRobotDFPlayerMini.h>
#include "Button.h"
#include "Weather.h"
#include "IllumiLed.h"
//#include "StatusLed.h" // not working
#include "config.h"

// ------------------
#define PIN_BTN 39

#define PIN_WLED 19

#define PIN_LED1 22
#define PIN_LED2 23
#define PIN_LED3 33

#define PIN_MP3_RX 25 // DFP PIN3 TX
#define PIN_MP3_TX 21 // DFP PIN2 RX
// ------------------

// APIで天気の取得を行う間隔(sec)
#define WEATHER_CHECK_INTREVAL 60
// SEの再生時間(sec)
#define SE_LENGTH 30

#define SPEED_LED1 3
#define SPEED_LED2 7
#define SPEED_LED3 9
/**
G19 デジタル入出力
G21 デジタル入出力
G22 デジタル入出力
G23 デジタル入出力
G25 デジタル入出力、アナログ入出力、ただし内部で6軸センサーのMPU6886のためにI2CのSCLで使用
G33 デジタル入出力、アナログ入力
 */

const uint8_t numOfLeds = 3;
const uint8_t ledPins[] = { 
  PIN_LED1,
  PIN_LED2,
  PIN_LED3
};
const uint8_t ledSpeeds[] = {
  SPEED_LED1,
  SPEED_LED2,
  SPEED_LED3
};

//SoftwareSerial mp3Serial(PIN_MP3_RX, PIN_MP3_TX); // RX, TX
HardwareSerial mp3Serial(1);
DFRobotDFPlayerMini myDFPlayer;

Button Btn = Button(PIN_BTN, true, 10);
//StatusLed statusLed = StatusLed(PIN_WLED);
Weather wBiz = Weather(API_KEY);
IllumiLed iLed = IllumiLed(3, ledPins);
WeatherData currentWeather;
bool ledOn = false;

uint8_t currentSeIndex = 1;

// ----------------------------------------------------

CRGB _ledData[1];
uint8_t _brightness;
int _diff;

void StatusLedTurnOff() {
    _ledData[0] = CRGB::Black;
    FastLED.show();
}

void StatusLedTurnOn() {
    _ledData[0] = CRGB::DeepSkyBlue;
    FastLED.show();
}

void StatusLedStatusErrorMode() {
    while(true) {
        _ledData[0] = CRGB::Red;
        FastLED.show();
        delay(100);
        _ledData[0] = CRGB::Black;
        FastLED.show();
        delay(100);
    }
}

// ----------------------------------------------------

bool setupWifi(uint8_t timeoutSec, bool isShowStatusLed = false) {
    unsigned long startTime = (millis() / 1000); 
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        if(((millis() / 1000) - startTime) > timeoutSec) {
           return false;
        }
        Serial.print(".");
        if(isShowStatusLed) iLed.blinkAll(1, 500, 128);
    }
    Serial.println("done");
    return true;
}

// ----------------------------------------------------

void updateStatusLedColorByWeather() {
    switch(currentWeather.condition) {
       case WeatherKind::Clear: 
           _ledData[0] = CRGB::FairyLight;
           currentSeIndex = 1;
           Serial.println("Clear, FairyLight"); 
           break;
//           case WeatherKind::Clear: _ledData[0] = CRGB::DeepSkyBlue; break;
       case WeatherKind::Clouds: 
           _ledData[0] = CRGB::DarkGray; 
           currentSeIndex = 1;
           Serial.println("Clouds, DarkGray"); 
           break;
       case WeatherKind::Rain: 
           _ledData[0] = CRGB::DarkBlue; 
//           currentSeIndex = 2;
           currentSeIndex = 1;
           Serial.println("Rain, DarkBlue"); 
           break;
       default: 
           _ledData[0] = CRGB::Crimson;
           currentSeIndex = 1;
           Serial.println("Others, Crimson");
    }
}

/** 定期的に天気情報を取得する */
void taskWeatherUpdate(void* param) {
    unsigned long prevMillis = 0;
    while(true) {
        if(!ledOn) {
            vTaskDelay(1000);
            continue;
        }
        // -------
        unsigned long now = millis();
        if ((now - prevMillis) < (WEATHER_CHECK_INTREVAL*1000)) {
            prevMillis = now;
            vTaskDelay(1000);
            continue;
        }
        if (WiFi.status() != WL_CONNECTED) {
            if(!setupWifi(5, false)) {
                prevMillis -= (WEATHER_CHECK_INTREVAL * 1000);
                vTaskDelay(1000);
                continue; 
            }
        }
        
        // update weather from api
        wBiz.update(currentWeather);
        Serial.printf("temp: %lf", currentWeather.temp);

        // update Status LED Color
        updateStatusLedColorByWeather();
    }
}

// ----------------------------------------------------
void taskPlaySE(void* param) {
    while(true) {
        myDFPlayer.play(currentSeIndex); // 天気に合わせたSEを再生
        vTaskDelay(SE_LENGTH * 1000);
    }
}

// ----------------------------------------------------

/** setup 内でエラー発生時の対応 */
void errorEnd() {
    iLed.blinkAll(3, 100, 128);
    iLed.turnOnAll(64);
    StatusLedStatusErrorMode();
    while(true){
        delay(0); // Code to compatible with ESP8266 watch dog.
    }
}

// ----------------------------------------------------

void setup() {
//    M5.begin(true, false, false); // (Serial, I2C, NeoPixel)
    Serial.begin(115200);
    pinMode(PIN_BTN, INPUT_PULLUP);

    // Setup Illumination LED(ESP32_LED_PWM)
    iLed.init();
    iLed.turnOnAll(128);

//    // Setup Status LED(FastLED)
//    statusLed.begin();
//    statusLed.turnOff();
    FastLED.addLeds<WS2812, PIN_WLED, GRB>(_ledData, 1);
    StatusLedTurnOff();

    ledOn = false;

    delay(100);
    if(!setupWifi(30, true)) { // try to connect in 30 sec
        Serial.println("wifi is not connected");
        errorEnd();
        return;
    }
    // Start Update Weather Data task in The Process CPU(Core0)
    xTaskCreatePinnedToCore(taskWeatherUpdate, "UpdateWeatherTask", 4096, NULL, 1, NULL, 0);

    wBiz.update(currentWeather);
    updateStatusLedColorByWeather();

    // Init DFPlayer Mini
    mp3Serial.begin(9600, SERIAL_8N1, PIN_MP3_RX, PIN_MP3_TX);
    delay(10);
    if (!myDFPlayer.begin(mp3Serial)) {  //Use softwareSerial to communicate with mp3.
        Serial.println(F("Unable to begin:"));
        Serial.println(F("1.Please recheck the connection!"));
        Serial.println(F("2.Please insert the SD card!"));
        errorEnd();
        return;
    }
    myDFPlayer.volume(10);
    // Start Update Weather Data task in The Application CPU(Core0)
    xTaskCreatePinnedToCore(taskPlaySE, "PlaySETask", 4096, NULL, 2, NULL, 0);
    myDFPlayer.play(1);

    Serial.println("Initialized.");
    iLed.turnOffAll();
    StatusLedTurnOff();
}

void loop() {
//    M5.update();
    Btn.read();

//    if(currentMillis % 20 == 0) {
//        statusLed.blink();
//        // statusLed.turnOff();
//    }]
    
//    if (M5.Btn.wasPressed()) {
    if (Btn.wasPressed()) {
        ledOn = !ledOn;
        Serial.printf("Btn: LED Status=%d\n", ledOn);
    }

    // --------
    
    if (!ledOn) { // LED turned Off
        // turn off Illumination leds
        iLed.turnOffAll();
//        // trun off Weather led
//        statusLed.turnOff();
        StatusLedTurnOff();
        delay(100);
        return;
    }
    
    // --------

    unsigned long currentMillis = (millis() / 10);
//    Serial.printf("CPU-MS: %lu\n", currentMillis);

    for(uint8_t i=0; i<numOfLeds; i++) {
        if(currentMillis % ledSpeeds[i] == 0) {
            iLed.lightYuragi(i);
        }
    }
    // TODO: blink status led slowly
//    if(currentMillis % 20 == 0) {
//        statusLed.blink();
//        // statusLed.turnOff();
//    }
//     StatusLedTurnOn();
    FastLED.show();

    delay(13);
}
