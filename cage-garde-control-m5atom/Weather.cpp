#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Weather.h"

Weather::Weather(String apiKey) {
    _apiKey = apiKey;
//    _xSemaphore = xSemaphoreCreateMutex();
}

bool Weather::update(WeatherData& value) {
    HTTPClient http;
    http.begin(endpoint + _apiKey); //URLを指定
    int httpCode = http.GET();  //GETリクエストを送信
    Serial.printf("httpCode: %d\n", httpCode);

    if (httpCode != 200) {
        http.end(); //リソースを解放
        value.isValid = false;
        return false;
    }

    String payload = http.getString();  //返答（JSON形式）を取得
    Serial.println(httpCode);
    Serial.println(payload);

    // jsonオブジェクトの作成
    DynamicJsonDocument doc(payload.length());
    auto error = deserializeJson(doc, payload);
    if (error) { // パースが成功したかどうかを確認
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        value.isValid = false;
        return false;
    }

    //各データを抜き出し
    const char* weather = doc["weather"][0]["main"].as<char*>();
    const double temp = doc["main"]["temp"].as<double>();
    Serial.print("weather:");
    Serial.println(weather);
    Serial.print("temperature:");
    Serial.println(temp-273.15);

    if (!strcmp(weather, "Clear")) {
        value.condition = WeatherKind::Clear;
    } else if (!strcmp(weather, "Clouds")) {
        value.condition = WeatherKind::Clouds;
    }
    value.temp = temp;
    value.isValid = true;
 
    http.end(); //リソースを解放
    return false;
}
