#include <Arduino.h>
#include "deneyap.h"
#include "WiFiESP32.h"
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <DHT.h>

#define WLAN_SSID       "DENEYAP"        
#define WLAN_PASS       "Deneyap.2023T"       
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                  
#define AIO_USERNAME    "yagizyagiz"         
#define AIO_KEY         "aio_iLri45U1UoOMKD5JBpuRmKuIrw3i"        

#define DHTPIN D0         
#define DHTTYPE DHT11

#define USE_MQTT true  

#define MQ_PIN A0        

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Publish sicaklik = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/sicaklik");
Adafruit_MQTT_Subscribe buton_durumu = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/buton-durumu");
Adafruit_MQTT_Publish gaz_seviyesi = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/gaz-seviyesi");
DHT dht(DHTPIN, DHTTYPE);

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) 
  {
    return;
  }
  
  Serial.print("Bulut sistemine baglaniliyor... ");
  uint8_t count = 3;
  
  while ((ret = mqtt.connect()) != 0 && count > 0) 
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("3 saniye icinde tekrar baglanti denemesi gerceklesecek...");
    mqtt.disconnect();
    delay(3000); 
    count = count - 1 ;
  }

  if (mqtt.connected()) {
    Serial.println("Bulut sistemine baglanildi!");
  } else {
    Serial.println("Bulut sistemine baglanilamadi! Sadece konsola yazilacak.");
  }
}

void setup() {
  Serial.begin(9600);
  delay(10);
  
  Serial.println(); Serial.println(); Serial.print(WLAN_SSID); Serial.print(" kablosuz agina baglaniliyor");
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Kablosuz aga baglanildi!");
  Serial.print("IP adresi: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&buton_durumu);
  dht.begin();
}

void loop() {
  if (USE_MQTT) {
    MQTT_connect();
  }
  Adafruit_MQTT_Subscribe *subscription;
  
  if (USE_MQTT) {
    while ((subscription = mqtt.readSubscription(2000))) 
    {
      if(subscription == &buton_durumu) 
      {
        Serial.println();
        Serial.print(F("Gelen mesaj: "));
        Serial.println((char *)buton_durumu.lastread);

        if (!strcmp((char*) buton_durumu.lastread, "AÇIK"))
        {
          writeBlueLed(HIGH);
        }
        else
        {
          writeBlueLed(LOW);
        }
      }
    }
  }

  float sicaklik_degeri = dht.readTemperature(); 
  Serial.print(F("\nSicaklik degeri gonderiliyor "));
  // Serial.print(dht.readHumidity(true));
  Serial.print("..."); 
  
  // Gas sensor reading
  int gaz_degeri = analogRead(MQ_PIN);
  Serial.print(F("\nGaz seviyesi: "));
  Serial.print(gaz_degeri);
  
  if(USE_MQTT && mqtt.connected()) {
    // Temperature publishing
    if(!sicaklik.publish(sicaklik_degeri)) {
      Serial.println(F(" Sicaklik gonderilemedi!"));
    }
    else {
      Serial.println(F(" Sicaklik gonderildi"));
    }
    
    // Gas value publishing
    if(!gaz_seviyesi.publish(gaz_degeri)) {
      Serial.println(F(" Gaz seviyesi gonderilemedi!"));
    }
    else {
      Serial.println(F(" Gaz seviyesi gonderildi"));
    }
  } else {
    Serial.println(F(" (Bulut baglantisi yok veya devre disi, sadece konsola yazildi)"));
  }

  delay(2000);
}
