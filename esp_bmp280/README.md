# ESP_BMP280
BMP280 -> MQTT Publisher

* `esp_bmp280.ino`\
  Main program. Connects to WiFi. Connects MQTT. Reads the BMP280
* `settings.h.template`\
  contains the settings you might need to configure this. Rename to `settings.h`
* `Adafruit_BMP280.[cpp|h]`\
  Adafruit BMP280 Library edited to only use i2c, and use i2c on GPIO0,2
 

## MQTT Message
  `{"Type":"BMP280","ID":"<mac-last6>","temperature":"DegC","pressure":"PresPas"}`

## PINS
  1. GND
  2. *NC*
  3. I2C SCL [GPIO2]
  4. HIGH (3.3V)
  5. I2C SDA [GPIO0]
  6. *NC*
  7. *NC*
  8. VCC (3.3V)

## Hardware
  * ESP8266 ESP01
  * Adafruit BMP280 I2C \
  https://www.adafruit.com/product/2651

## Dependencies
  * Adafruit MQTT Library
  * Arduino JSON 
  * Adafruit Sensor library
	
    
  * Adafruit BMP280 Library, **included** \
  	*Edited:* Using i2c(only) on GPIO0 and GPIO2
	
