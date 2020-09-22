//#include "StatusLed.h"
//
//StatusLed::StatusLed(uint8_t pin) {
//    _pin = pin;
//    _ledData[0] = CRGB::Black;
//}
//
//void StatusLed::begin() {
//    FastLED.addLeds<WS2812, this->_pin, GRB>($this->ledData, 1);
//    _brightness = 0;
//    _diff = 1;
//}
//
//void StatusLed::turnOff() {
//    _ledData[0] = CRGB::Black;
//    FastLED.show();
//}
//
//void StatusLed::turnOn() {
//    _ledData[0] = CRGB::White;
//    FastLED.show();
//}
//
//void StatusLed::blink() {
//    wLeds[0].r = brightness;
//    wLeds[0].g = brightness;
//    wLeds[0].b = brightness;
//    FastLED.show();
//
//    if (brightness == 0) {
//      diff = 1;
//    } else if (brightness == 255) {
//      diff = -1;
//    }
//   
//    brightness += diff;
//}
