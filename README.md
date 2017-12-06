# ESPHome
ESP Code for my "home-ESPs"

Ardiono IDE .ino files for the ESP8266s I use around the house.

## ESP_BMP280
  Adafruit BMP280 (Temp+Pressure) Sensor publishing on MQTT over WiFi via ESP
  
  `{"Type":"BMP280","ID":"<mac-last6>","temperature":"DegC","pressure":"PresPas"}`
  
## ESP_P1
  Dutch Smart Meter (Gas/Elecricity) reader publishing on MQTT over WiFi via ESP
  
  `{"Type":"P1","ID":"<mac-last6>","electricity":"USAGE1;USAGE2;RETURN1;RETURN2;CURRUSE;CURRPOD","gas":"GAS"}`
  


