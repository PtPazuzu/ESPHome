/*
    BMP280 -> MQTT Publisher

    MQTT Message:
      {"Type":"BMP280","ID":"<mac-last6>","temperature":"DegC","pressure":"PresPas"}

    PINS
      1 GND
      2 NC
      3 I2C SCL [GPIO2]
      4 HIGH(3.3V)
      5 I2C SDA [GPIO0]
      6 NC
      7 NC
      8 VCC(3.3V)

    Using
      Adafruit BMP280 I2C 
      https://www.adafruit.com/product/2651

   Dependencies
     Adafruit MQTT Library
     Arduino JSON
	 Adafruit Sensor library
	 
	 Adafruit BMP280 Library, included.
	   Edited: Using i2c(only) on GPIO0 and GPIO2

   MIT License

   (C) 2017 PtPazuzu

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.


   Loosely based on various Adafruit examples

   Adafruit invests time and resources providing this open source code,
   please support Adafruit and open-source hardware by purchasing
   products from Adafruit!
   https://www.adafruit.com

*/

#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "Adafruit_BMP280.h" // Edited i2c on Wire.begin(0,2)
#include "settings.h"

WiFiClient wclient;
Adafruit_MQTT_Client mqtt(&wclient, S_MQTT_HOST, S_MQTT_PORT, S_MQTT_CLIENT, S_MQTT_USER, S_MQTT_PASS);

Adafruit_MQTT_Publish mqtt_pub = Adafruit_MQTT_Publish(&mqtt, S_MQTT_PTOPIC);

Adafruit_BMP280 bme; // I2C

String UseID = "";

void MQTT_connect();
void pub_mqtt(String k, String v);

void setup() {
  uint8_t MAC_array[6];
  char ID[6] = "";

  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);

  // Client Only
  WiFi.mode(WIFI_STA);

  WiFi.macAddress(MAC_array);
  for (int i = 3; i < sizeof(MAC_array); ++i) {
    sprintf(ID, "%s%02x", ID, MAC_array[i]);
  }
  UseID = String(String(ID));

  WiFi.begin ( S_WIFI_SSID, S_WIFI_PASSWORD );

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); // Keep trying
  }

  if (!bme.begin()) {
    MQTT_connect();
    pub_mqtt("ERROR", "BMP Not Found");
  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
}

void pub_mqtt(String k, String v) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject(); // Create JSON object
  root["Type"] = "BMP280";
  root["ID"] = UseID;
  root[k] = v;
  char buffer[240];
  root.printTo(buffer, sizeof(buffer));
  if (! mqtt_pub.publish(buffer)) {
    // Failed
  }
}

void pub_mqttValues(String temp, String pressure) {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject(); // Create JSON object
  root["Type"] = "BMP280";
  root["ID"] = UseID;
  root["temperature"] = temp;
  root["pressure"] = pressure;

  char buffer[240];
  root.printTo(buffer, sizeof(buffer));
  if (! mqtt_pub.publish(buffer)) {
    // Failed
  }
}

void loop() {
  MQTT_connect();
  pub_mqttValues(String(bme.readTemperature()), String(bme.readPressure()));

  delay(S_TIMEOUT); // 10s Delay between sends
}

