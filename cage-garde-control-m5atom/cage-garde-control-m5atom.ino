//#include <M5Atom.h> // official library is not working with additional neopixels...
#include <FastLED.h>
#include "Button.h"
#include "Weather.h"
#include "StatusLed.h"
#include "config.h"

#define PIN_BTN 39
#define PIN_WLED 33
#define PIN_LED1 25
#define PIN_LED2 19
#define PIN_LED3 21
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
StatusLed statusLed = StatusLed(PIN_WLED);
Weather wBiz = Weather(API_KEY);
bool ledOn = false;

// ----------------------------------------------------

uint8_t value = 0; // variable to keep the actual value
int ledpin = 9; // light connected to digital pin 9
float x = 0.1;
float x1 = 0.1;

uint8_t chaos() {
    // 1/f yuragi
    x = x1;
    if(x < 0.5){
        x = x + 2 * x * x;
    }
    else {
        x = x - 2 * (1.0 - x) * (1.0 - x);
    }
  
    if(x < 0.08){
        x = (float)(random(20,80))/1023;
    }
    if(x > 0.995){
        x = (float)(random(128,253))/1023;
    }
  
    x1 = x;
    x = x * 255;
  
    return (uint8_t)x;
}

// ----------------------------------------------------

void setupWifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(200);
    }
    Serial.println("done");
}

// ----------------------------------------------------

void setup() {
//    M5.begin(true, false, false); // (Serial, I2C, NeoPixel)
    Serial.begin(115200);
    pinMode(PIN_BTN, INPUT_PULLUP);

    // Setup Illumination LED(ESP32_LED_PWM)
    for(uint8_t i=0; i<numOfLeds; i++) {
        ledcSetup(i, 12800, 8);
        ledcAttachPin(ledPins[i], i);
    }

    // Setup Status LED(FastLED)
    statusLed.begin();
    statusLed.turnOff();

    ledOn = false;

    setupWifi();

    Serial.println("Initialized.");
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
        for(uint8_t i=0; i<numOfLeds; i++) {
            ledcWrite(i, 0);
        }
        // trun off Weather led
        statusLed.turnOff();
        delay(100);
        return;
    }
    
    // --------

    unsigned long currentMillis = (millis() / 10);
//    Serial.printf("CPU-MS: %lu\n", currentMillis);

    for(uint8_t i=0; i<numOfLeds; i++) {
        if(currentMillis % ledSpeeds[i] == 0) {
            value = chaos();
            ledcWrite(i, value);
            Serial.printf("LED%u: %u\n", i, value);
        }
    }
    if(currentMillis % 20 == 0) {
        statusLed.blink();
        // statusLed.turnOff();
    }

    delay(13);
}
