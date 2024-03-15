#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN 5 //D1
#define DHTTYPE DHT11

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

//Connection Config
String ssid = "Harapan Bersama";
String pass = "poltekharber"; //Kosongi bila tanpa password

//ThingSpeak Config
String host = "api.thingspeak.com";
String writeAPIKey = "TU2UXYCNSIJFB84Y";
String request_string;

void setup()
{
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.begin(ssid, pass);
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  dht.begin();
}

void loop()
{
  delay(2000);
  float h = dht.readHumidity(); //Kelembaban
  float t = dht.readTemperature(); //Celcius

  send_data(t, h);

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

void send_data(float suhu, float hum)
{
  if (client.connect(host, 80))
  {
    request_string = "/update?key=" + writeAPIKey 
                    + "&field1=" + suhu
                    + "&field2=" + hum;
                    
    Serial.println(String("GET ") + request_string + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
                 
    client.print(String("GET ") + request_string + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    
    while (client.available() == 0)
    {
      if (millis() - timeout > 5000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }

    Serial.println();
    Serial.println("Closing connection");
  }
}
