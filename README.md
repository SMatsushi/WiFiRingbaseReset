# WiFiRingbaseReset

## System Configuration Diagram
![System Configuration Diagram](/images/RBR_System.png)

## Devices on the Board
* ESP32 VE32 - devkit - ESP32-WROVER-E (Arduino)
* OLED SSD1306
* Relay Module (5V operation)

## Circuit Diagram

![Circuit Diagram](/RingReset.png)
File:
* png image file: RinReset.png 
* source: RingReset.ce3 (for schmatic editor 'BSch3V')

On the circuit diagram, 'CN1' is connected to an LED for testing. It is connected to the both terminals of the reset switch in 'Ring Base Station' during operation.

## Arduino IDE download link
[https://www.arduino.cc/en/software/](https://www.arduino.cc/en/software/)

## Arduino Source Code
The Arduino code is 'WiFiRingbaseReset/WiFiRingbaseReset.ino'.

>[!Note]
>* WiFi SSID and password are hard coded in WiFiRingbaseReset.ino.
>* Websocket is used to push the reset status to the web browser.
>* It periodically resets itself (a day in the current source), which recovers from the lost WiFi connection.
>* Status (Reset Status, A progress bar to show liveness, IP address) are displayed on the OLED display of the device.

### Related Links
* OLED control code: [https://github.com/ThingPulse/esp8266-oled-ssd1306/](https://github.com/ThingPulse/esp8266-oled-ssd1306/) 
  * [ OLED control keyword ](https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/keywords.txt) for the code above.
* ESP-IDF Programming Guide
  * [ ESP32 Self-reset API ](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/misc_system_api.html)
  * [ ESP32 Watchdogs ](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/wdts.html)

## USB Serial board to write ESP32
　[https://amzn.to/3Ac7aK9](https://amzn.to/3Ac7aK9) 　HiLetgo FT232RL FTDI Mini USB to TTL Serial Converter Adapter Module 3.3V 5.5V FT232R Breakout FT232RL USB to Serial Mini USB to TTL Adapter Board for Arduino

## Arduino tool setup example for ESP32 programming on Macbook
 /dev/cu.usbserial-0001    115200 baud, newline

