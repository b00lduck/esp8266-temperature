#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//AP definitions
#define AP_SSID "AP-EG"
#define AP_PASSWORD "ap password"

#define REPORT_INTERVAL 60 // in sec

#define ONE_WIRE_BUS 2  // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

void setup() {
  Serial.begin(115200); 
  wifiConnect();

  float temp;
  
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    Serial.print("Temperature: ");
    Serial.println(temp);
  } while (temp == 85.0 || temp == (-127.0));
  
  sendTeperature(temp);
  
  ESP.deepSleep(10000000);
   
}

void loop() {
}

void wifiConnect() {
    Serial.print("Connecting to AP");
    WiFi.begin(AP_SSID, AP_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");  
}

void sendTeperature(float temp) {  
   WiFiClient client;
   
   while(!client.connect("192.168.2.107", 8080)) {
    Serial.println("connection failed");
    wifiConnect(); 
  }

  char postData[8];
  sprintf((char*)&postData, "%3.2f", temp);
   
  client.println("POST /temperature/esp8266test1 HTTP/1.1");
  client.println("Host: 192.168.2.107");
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(strlen(postData));
  client.println();
  client.println(postData);
               
  delay(100);
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("Connection closed");
}
