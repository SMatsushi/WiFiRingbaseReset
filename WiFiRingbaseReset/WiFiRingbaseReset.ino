/*********
  Acknowledgement:
   Rui Santos
   Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
*********/

/*
 * Current Hardware MCU: 
 *     ESP32-WROVER-E (M-15674)
 *        ESP-32 VE32 Devkit C 8MB, uUSB Powered, with 3.3V power output
 *     OLED: 
 *       0.96 Inch OLED Module 12864 128x64 Driver SSD1306 I2C Control
 *       Self-Luminous Display Board Compatible with Arduino Raspberry PI (Blue and Yellow)
 *     Relay unit: 5V 1 channel, 10A (30V DC or 250V AC)
 *       Unit connection: Vcc to 5V, GND: in1 to GPIO-26, in2 to GPIO-27
 *       Need 5V control swing - 2SC1815 converts 3.3V p-p from ESP32 to 5V p-p with polality inversion.
 *       
 *   OLED Driver: https://github.com/ThingPulse/esp8266-oled-ssd1306
 *   Relay Sample: http://marchan.e5.valueserver.jp/cabin/comp/jbox/arc214/doc21406.html
 *   WebSocket Sample: https://randomnerdtutorials.com/esp32-websocket-server-arduino/
 */

// Import required libraries
#include <esp_system.h>
// #include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SSD1306Wire.h>
// #include <SSD1306wire.h>


// Hardcoded WiFi credentials
const char* ssid = "FooBarNet";
const char* password = "mySecretPass";

const bool OFF = 0;
const bool ON = 1; 
bool resetState = OFF;
const int ledPin = 26;  // Relay output on the breadboard

// Setup OLED
// OLED Connection: SD = GPIO-21, SCL = GPIO-22
SSD1306Wire display(0x3c, SDA, SCL);
// SSD1306wire display(0x3c, SDA, SCL);
const int oledXpixel = 128;
const int oledYpixel = 64;

// definition of OLED display position
const int16_t ipY = 0;
const int16_t ipH = 10; // ip Font Height
const uint8_t* ipFontP = ArialMT_Plain_10;
const int16_t infoY = 10;
const int16_t infoH = 10;
const uint8_t* infoFontP = ArialMT_Plain_10;
const int16_t actY = 20;
const int16_t actH = 26;  // 2 pixel for font undershoot
const uint8_t* rstFontP = ArialMT_Plain_16;
const uint8_t* actFontP = ArialMT_Plain_24;
const uint16_t progY = 48; // progress bar Y pos
const uint16_t progH = 6;  // progress bar Height
const int16_t macY = 54;
const int16_t macH = 10;
const uint8_t* macFontP = ArialMT_Plain_10;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP WebSocket Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
   .state-reset {
     font-size: 1.5rem;
     color:#ff8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Ring Base Station</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Reset</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    var css_cur;
    var css_new;
    const st_list = document.getElementById("state").classList;
    if (event.data == "1"){
      state = "Resetting";
      css_cur ="state";
      css_new ="state-reset";
    }
    else{
      state = "Running";
      css_cur ="state-reset";
      css_new ="state";
    }
    st_list.remove(css_cur);
    document.getElementById('state').innerHTML = state;
    st_list.add(css_new);
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>
)rawliteral";

char charbuf[128];  // char buffer for oled print

void notifyClients() {
  char *chp;
  if (resetState == ON) {
    chp = (char *)"Resetting";
  } else {
    chp = (char *)"Running";
  }
  printConsole(12, actY, actH, actFontP, chp);
  ws.textAll(String(resetState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      resetState = ON;
      notifyClients();
      delay(3000);   // causing more conflicts
      // delay(1000);
      resetState = OFF;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      sprintf(charbuf, "Ws#%u con. %s", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      sprintf(charbuf, "Ws#%u disconnected", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      sprintf(charbuf, "Ws#%u pong frm %s", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_ERROR:
      sprintf(charbuf, "Ws#%u event err %s", client->id(), client->remoteIP().toString().c_str());
      break;
  }
  printConsole(0, infoY, infoH, infoFontP, charbuf);
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (resetState){
      return "Resetting";
    } else {
      return "Running";
    }
  }
  return String();
}

void printConsole(int16_t  oledPosX, int16_t oledPosY, int16_t fontHeight, const uint8_t* fontP, char* charP) {
   // display to serial console
   Serial.println(charP);
   
   // display to OLED panel
   // clear buffer for the area first
   display.setColor(BLACK);
   display.fillRect(oledPosX, oledPosY, oledXpixel - oledPosX, fontHeight+1);
   display.setColor(WHITE);
   //
   display.setFont(fontP);
   display.drawString(oledPosX, oledPosY, charP);
   // output the buffer
   display.display();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // setup OELD
  display.init();
  display.clear(); // clear the display buffer
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  // Get MAC address for WiFi station
  uint8_t baseMac[6];
  esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  sprintf(charbuf, "MAC: %02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
  printConsole(0, macY, macH, macFontP, charbuf);  // posX, posY, fontHeight, charbuf
  delay(5000); // causing more conflicts
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    printConsole(0, ipY, ipH, ipFontP, (char *)"Connecting to WiFi...");
  }

  // Print ESP Local IP Address
  sprintf(charbuf, "IP: %s", WiFi.localIP().toString().c_str());
  printConsole(0, ipY, ipH, ipFontP, charbuf);
  
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

int counter = 0;
int progress = 0;   // percent
#define COUNT_LIMIT 15000  // count value to update progress and reset count
#define PROG_INCR 4       // increment of progress
int reset_counter = 0;

//****
// Reboot itself as it losts WiFi connection.
//****

// #define RESET_LIMIT 10   // after repeating count limit this time, reset myself, 35-37 sec after boot (for testing with more conflicts)
// #define RESET_LIMIT 15   // after repeating count limit this time, reset myself, 52-56 sec after boot (for testing with more conflicts)
// #define RESET_LIMIT 20   // after repeating count limit this time, reset myself, 70-75 sec after boot (for testing)
#define RESET_LIMIT 23600   // after repeating count limit this time, reset myself, one day after boot (for advanced testing)
// #define RESET_LIMIT 71010   // after repeating count limit this time, reset myself, three days after boot (for deployment?)

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, resetState);
  if (++counter > COUNT_LIMIT) {
    counter = 0;
    display.setColor(WHITE);
    display.drawProgressBar(4, progY, 120, progH, progress);
    display.display();
    progress += PROG_INCR;
    if (progress > 100) {
      progress = 0;
      display.setColor(BLACK);
      display.fillRect(4, progY, 120, progH);
      if (++reset_counter > RESET_LIMIT) {
          printConsole(12, actY, actH, rstFontP, "Restarting self!");
          reset_counter = 0;
          delay(1000);
          esp_restart();
      }
    }
  }
}
