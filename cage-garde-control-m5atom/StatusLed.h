#include <Arduino.h>
#include <FastLED.h>

class StatusLed {
public:
    StatusLed(uint8_t pin);
    void begin();
    void turnOff();
    void turnOn();
    void blink();

private:
    uint8_t _pin;
    CRGB _ledData[1];
    uint8_t _brightness;
    int _diff;
};
