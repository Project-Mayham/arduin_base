#include <Arduino.h>
#include "deneyap.h"
#include <DHT.h>

#define DHTPIN A0
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(9600);
    dht.begin();
}

void loop() {
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    delay(2000);
}
