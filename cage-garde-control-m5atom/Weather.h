#include <Arduino.h>
#include <ArduinoJson.h>

enum class WeatherKind { Thunderstorm, Drizzle, Rain, Snow, Clear, Clouds };
// https://openweathermap.org/weather-conditions

struct WeatherData {
  WeatherKind condition;
  double temp;
  bool isValid;
} typedef GPSDateTime;


class Weather {
public:
    Weather(String apiKey);
    bool update(WeatherData& value);
private:
    const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=tokyo,jp&units=metric&APPID=";
    String _city;
    String _apiKey;
    
    static const size_t _capacity = JSON_OBJECT_SIZE(13) 
      + JSON_OBJECT_SIZE(2) // coord
      + JSON_OBJECT_SIZE(4) // weather
      + JSON_OBJECT_SIZE(6) // main
      + JSON_OBJECT_SIZE(2) // wind
      + JSON_OBJECT_SIZE(5) // sys
      + 256;
    StaticJsonDocument<_capacity> _doc;
};
