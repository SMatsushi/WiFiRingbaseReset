# WiFiRingbaseReset

## System Configuration Diagram
![System Configuration Diagram](/images/RBR_System.png)

## Practical Meaning of This Device
* Ring security system is connected to the internet and reports security issues to mobile apps or PC browsers.
* The following Ring security cameras are directly connected to local WiFi.
  *  Ring doorbell, Floodlight Camera, Spotlight Camera, etc.
* The following sensors are, however, connected to Ring Basestation with its proprietary network (Z-wave).
  * Ring Keypad, Digital Doorlock (Yale YRD256, etc), Motion detectors, Window/door status (open/close) sensors, Smork/CO detector listeners, etc.
  * If the internet connection is lost, the Ring Basestation goes to backup mode with AT&T Cellular network and provides only some serious security issues.
  * When the internet connection is recovered, the Ring Basestation sometimes stays in AT&T Cellular network connection.
  * A custom BroadBand router on the fanless x86 server detects internet connection loss and triggers the following procedures. As a part of the sequence, this WiFiRingbaseReset is triggered to recover its internet connection from the backup cellular connection for restoring the full functionality:
    * The custom BroadBand router (BBrouter) is connected Xfinity cable modem.
    * 'recovWAN.py' running on BBrouter periodically (every 5 minutes) by cron.tab and performs followings:
      * 'recovWAN.py' detects the internet connection loss when neither of the pings to DNS servers responds.
      * Resets the internet port with "systemctl restart NetworkManager", since the Xfinity modem provides after its autonomous reboots/updates an abnormal IP address, such as 192.168.100.10 (instead of the correct global IP like 76.133.103.xxx), to the BBrouter.
      * If the internet connection remains lost after multiple tries of the internet port reset, 'recovWAN.py' triggers an Arduino device connected to BBrouter with a USB port and toggles the DC power line of the Xfinity modem to reboot the modem.
      * When 'recovWAN.py' detects the internet connection recovery, it triggers 'toggleESP32.py' to reset the Ring Basestation with this WiFiRingbaseReset device.
  * Related files on the custom BBrouter will be uploaded to my GitHub soon.
## Devices on the Board
* ESP32 VE32 - devkit - ESP32-WROVER-E (Arduino)
* OLED SSD1306
* Relay Module (5V operation)

## Circuit Diagram

![Circuit Diagram](/RingReset.png)
File:
* png image file: ./RingReset.png 
* source: ./RingReset.ce3 (for schmatic editor 'BSch3V')

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

