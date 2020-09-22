#include <Arduino.h>

class Weather {
public:
    Weather(String apiKey);
    bool update();
private:
    const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=tokyo,jp&APPID=";
    String _apiKey;
};
