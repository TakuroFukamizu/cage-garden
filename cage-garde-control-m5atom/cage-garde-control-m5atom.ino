//#include <M5Atom.h> // official library is not working with additional neopixels...
#include <FastLED.h>
#include <WiFi.h>
#include "Button.h"
#include "Weather.h"
#include "IllumiLed.h"
//#include "StatusLed.h" // not working
#include "config.h"

// ------------------
#define PIN_BTN 39

#define PIN_WLED 33

#define PIN_LED1 25
#define PIN_LED2 19
#define PIN_LED3 21

#define PIN_MP3_RX 22
#define PIN_MP3_RX 23
// ------------------

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


Button Btn = Button(PIN_BTN, true, 10);
//StatusLed statusLed = StatusLed(PIN_WLED);
Weather wBiz = Weather(API_KEY);
IllumiLed iLed = IllumiLed(3, ledPins);
WeatherData currentWeather;
bool ledOn = false;

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

// ----------------------------------------------------

bool setupWifi(uint8_t timeoutSec) {
    unsigned long startTime = (millis() / 1000); 
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        if(((millis() / 1000) - startTime) > timeoutSec) {
           return false;
        }
        Serial.print(".");
        iLed.blinkAll(1, 500, 128);
    }
    Serial.println("done");
    return true;
}

// ----------------------------------------------------

/** 定期的に天気情報を取得する */
void taskWeatherUpdate(void* param) {
    unsigned long prevMillis = 0;
    while(true) {
        unsigned long now = millis();
        if ((now - prevMillis) < 10000) { // every 10 sec
            prevMillis = now;
            vTaskDelay(1000);
            continue;
        }
        // update weather from api
        wBiz.update(currentWeather);
        Serial.printf("weather: %d", currentWeather.temp);
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
    if(!setupWifi(30)) { // try to connect in 30 sec
        Serial.println("wifi is not connected");
        iLed.blinkAll(3, 100, 128);
        iLed.turnOnAll(64);
        return;
    }
    // Start Update Weather Data task in The Process CPU(Core0)
    xTaskCreatePinnedToCore(taskWeatherUpdate, "UpdateWeatherTask", 4096, NULL, 1, NULL, 0);

    wBiz.update(currentWeather);
    Serial.printf("weather: %d", currentWeather.temp);

    Serial.println("Initialized.");
    iLed.blinkAll(3, 500, 128);
    iLed.turnOffAll();
}

void loop() {
//    M5.update();
    Btn.read();
    
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
//    if(currentMillis % 20 == 0) {
//        statusLed.blink();
//        // statusLed.turnOff();
//    }
     StatusLedTurnOn();

    delay(13);
}
