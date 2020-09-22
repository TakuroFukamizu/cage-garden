#include <Arduino.h>

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
};
