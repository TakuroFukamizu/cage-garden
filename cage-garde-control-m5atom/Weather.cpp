#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Weather.h"

Weather::Weather(String apiKey) {
    _apiKey = apiKey;
}

bool Weather::update() {
    HTTPClient http;
    http.begin(endpoint + _apiKey); //URLを指定
    int httpCode = http.GET();  //GETリクエストを送信
    Serial.printf("httpCode: %d\n", httpCode);

    if (httpCode != 200) {
        http.end(); //リソースを解放
        return false;
    }

    String payload = http.getString();  //返答（JSON形式）を取得
    Serial.println(httpCode);
    Serial.println(payload);

    //jsonオブジェクトの作成
    DynamicJsonBuffer jsonBuffer;
    String json = payload;
    JsonObject& weatherdata = jsonBuffer.parseObject(json);

    //パースが成功したかどうかを確認
    if(!weatherdata.success()){
        Serial.println("parseObject() failed");
    }

    //各データを抜き出し
    const char* weather = weatherdata["weather"][0]["main"].as<char*>();
    const double temp = weatherdata["main"]["temp"].as<double>();
    Serial.print("weather:");
    Serial.println(weather);
    Serial.print("temperature:");
    Serial.println(temp-273.15);
 
    http.end(); //リソースを解放
}
