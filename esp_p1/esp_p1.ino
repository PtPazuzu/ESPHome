#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <ArduinoJson.h>  
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "settings.h"

WiFiClient wclient;
Adafruit_MQTT_Client mqtt(&wclient, S_MQTT_HOST, 1883, S_MQTT_CLIENT, S_MQTT_USER, S_MQTT_PASS);

Adafruit_MQTT_Publish mqtt_pub = Adafruit_MQTT_Publish(&mqtt, S_MQTT_PTOPIC);

char lastmessage_recvd[100];
String lastmessage;

const int IDX_P1 = 16;
const int IDX_G = 17;

char count = 0;
int incomingByte = 0;
int U1_pos;
int U2_pos;
int R1_pos;
int R2_pos;
int P1_pos;
int C1_pos;
int G1_pos;
String inputString;
String U1;
String U2;
String R1;
String R2;
String P1;
String C1;
String G1;
String outputString;
String lastString;
String UseID = "";


String CreateHWID() {
  uint8_t MAC_array[6];
  char ID[6] = "";  
  
  WiFi.macAddress(MAC_array);
  for (int i = 3; i < sizeof(MAC_array); ++i) {
    sprintf(ID, "%s%02x", ID, MAC_array[i]);
  }
  return String(String(ID));
}

void setup ( void ) {
  // My P1 Meter uses an inverted 7E1 output
  Serial.begin ( 9600, SERIAL_7E1 );

  // Client only
  WiFi.mode(WIFI_STA);

  // HardwareID
  UseID = CreateHWID();

  // Connect
  WiFi.begin ( S_WIFI_SSID, S_WIFI_PASSWORD );
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 ); // Keep trying  
  }
}

void pub_mqttValues(String electricity, String gas) {
   StaticJsonBuffer<200> jsonBuffer;
   JsonObject& root = jsonBuffer.createObject(); // Create JSON object
   root["Type"] = "P1";
   root["ID"] = UseID;
   root["electricity"] = electricity;
   root["gas"] = gas;

   char buffer[240];
   
   root.printTo(buffer, sizeof(buffer));
   
   if (! mqtt_pub.publish(buffer)) {
     // failed..
   } 
}

void CheckSerial(){
  while (Serial.available() > 0) {  
    incomingByte = Serial.read();    
    char inChar = (char)incomingByte;
    inputString += inChar; 
   }

   if (inputString.length() > 3048) {
     inputString = "";
   }
  
   //If output from Smart meter is long enough, process it. Length needs to be checked individually just in case
   if (inputString.endsWith("!")) {
      lastString = inputString;
      // Usage 1
      U1_pos = inputString.indexOf("1-0:1.8.1", 0);
      U1 = inputString.substring(U1_pos + 10, U1_pos + 19);

      // Usage 2
      U2_pos = inputString.indexOf("1-0:1.8.2", U1_pos + 1);
      U2 = inputString.substring(U2_pos + 10, U2_pos + 19);

      // Return 1
      R1_pos = inputString.indexOf("1-0:2.8.1", U2_pos + 1);
      R1 = inputString.substring(R1_pos + 10, R1_pos + 19);

      // Return 2
      R2_pos = inputString.indexOf("1-0:2.8.2", R1_pos + 1);
      R2 = inputString.substring(R2_pos + 10, R2_pos + 19);

      // Consume
      C1_pos = inputString.indexOf("1-0:1.7.0", R2_pos + 1);
      C1 = inputString.substring(C1_pos + 10, C1_pos + 17);

      // Production
      P1_pos = inputString.indexOf("1-0:2.7.0", C1_pos + 1);
      P1 = inputString.substring(P1_pos + 10, P1_pos + 17);

      // Gas
      G1_pos = inputString.indexOf("0-1:24.3.0", P1_pos + 1);
      G1_pos = inputString.indexOf("(m3)", G1_pos + 1);      
      G1 = inputString.substring(G1_pos + 7, G1_pos + 16);

      int iU1, iU2, iR1, iR2, iC1, iP1, iG1;
      iU1 = ceil(U1.toFloat() * 1000);
      iU2 = ceil(U2.toFloat() * 1000);
      iR1 = ceil(R1.toFloat() * 1000);
      iR2 = ceil(R2.toFloat() * 1000);
      iC1 = ceil(C1.toFloat() * 1000);
      iP1 = ceil(P1.toFloat() * 1000);
      String P1Combined = String(iU1) + ";"  + String(iU2) + ";" + String(iR1) + ";" + String(iR2) + ";" + String(iC1) + ";" + String(iP1);
      iG1 = ceil(G1.toFloat() * 1000);

      pub_mqttValues(P1Combined, String(iG1));
      
      inputString = "";      
   }  
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
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

void loop ( void ) {
  MQTT_connect();    
  CheckSerial();      
}
