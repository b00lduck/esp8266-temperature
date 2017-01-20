#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

OneWire oneWire(2);
DallasTemperature DS18B20(&oneWire);

void setup() {
  Serial.begin(115200); 

  Serial.print(THERMOMETER_ID);
  Serial.print(" @ version ");
  Serial.println(VERSION_ID);
  
  wifiConnect();

  float temp;
  
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    Serial.print("Temperature: ");
    Serial.println(temp);
  } while (temp == 85.0 || temp == (-127.0));
  
  sendTeperature(temp);
  
  ESP.deepSleep(DEEP_SLEEP);
  delay(100);  
}

void loop() {
  // nothing to do here
}

void wifiConnect() {
  int retries = MAX_WIFI_RETRIES;
  Serial.println("");
  Serial.print("Connecting to AP");
  WiFi.begin(AP_SSID, AP_PASSWORD);
  while ((WiFi.status() != WL_CONNECTED) && (retries--)) {
    delay(1000);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connection not possible, will retry later");        
    ESP.deepSleep(DEEP_SLEEP_WIFI_FAILURE);
    delay(100);  
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  

}

void sendTeperature(float temp) {  
   WiFiClient client;
   
   while(!client.connect(SERVER_IP, SERVER_PORT)) {
    Serial.println("connection failed");
    return;
  }

  int d1 = temp;
  float f2 = temp - d1;
  int d2 = trunc(f2 * 100);
  char postData[20];

  sprintf(postData, "%d.%02d", d1, d2);
  Serial.println(postData);
   
  client.printf("PUT /thermometer/%s HTTP/1.1\n", THERMOMETER_ID);
  client.printf("Host: %s\n", SERVER_IP);
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.printf("Content-Length: %d\n", strlen(postData));
  client.println();
  client.print(postData);
               
  delay(100);
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("Connection closed");
}
