#include <Arduino.h>

#define BUF_NUM_LED 3

class IllumiLed {
public:
    IllumiLed(const uint8_t numOfLed, const uint8_t pins[]);
    void init();
    void turnOffAll();
    void turnOnAll(uint8_t bri);
    void blinkAll(uint8_t count, uint8_t speed, uint8_t bri);
    void lightYuragi(uint8_t indexOfLed);

private:
    uint8_t _numOfLed;
    uint8_t _ledPins[BUF_NUM_LED] = {0, 0, 0};
    uint8_t _ledSpeed[BUF_NUM_LED] = {0, 0, 0};
    uint8_t _value = 0;

    float x = 0.1;
    float _fValue[BUF_NUM_LED] = {0.1, 0.1, 0.1};

    uint8_t chaos(uint8_t indexOfLed);
};
