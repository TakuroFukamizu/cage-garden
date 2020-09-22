#include "IllumiLed.h"

IllumiLed::IllumiLed(const uint8_t numOfLed, const uint8_t pins[]) {
    _numOfLed = numOfLed;
    for (uint8_t i = 0; i < numOfLed; i++) {
        _ledPins[i] = pins[i];
    }
}

void IllumiLed::init() {
    for(uint8_t i=0; i<_numOfLed; i++) {
      ledcSetup(i, 12800, 8);
      ledcAttachPin(_ledPins[i], i);
    }
}

void IllumiLed::turnOffAll() {
    for(uint8_t i=0; i<_numOfLed; i++) {
        ledcWrite(i, 0);
    }
}

void IllumiLed::turnOnAll(uint8_t bri) {
    for(uint8_t i=0; i<_numOfLed; i++) {
        ledcWrite(i, bri);
    }
}

void IllumiLed::blinkAll(uint8_t count, uint8_t speed, uint8_t bri) {
    for(uint8_t i=0; i<count; i++) {
        this->turnOnAll(bri);
        delay(speed);
        this->turnOffAll();
        delay(speed);
    }
}

void IllumiLed::lightYuragi(uint8_t indexOfLed) {
    _value = chaos(indexOfLed);
    ledcWrite(indexOfLed, _value);
    Serial.printf("LED%u: %u\n", indexOfLed, _value);
}

uint8_t IllumiLed::chaos(uint8_t indexOfLed) {
    // 1/f yuragi
    x = _fValue[indexOfLed];
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
  
    _fValue[indexOfLed] = x;
    x = x * 255;
  
    return (uint8_t)x;
}
