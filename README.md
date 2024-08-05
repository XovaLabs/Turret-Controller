# Documentation for the ESP8266 Web Server and WebSocket Turret Controller

This code sets up an ESP8266 microcontroller to serve web pages and handle WebSocket connections using the `ESPAsyncWebServer` library. It allows users to interact with the device through a web interface and WebSocket connections.

## Included Libraries
```cpp
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>
```
- `Arduino.h`: Core library for Arduino.
- `ESP8266WiFi.h`: Library to manage Wi-Fi connections on the ESP8266.
- `ESPAsyncWebServer.h`: Asynchronous web server library for handling HTTP requests.
- `LittleFS.h`: File system library for the ESP8266.
- `WebSocketsServer.h`: Library for handling WebSocket connections.

## Network Credentials
```cpp
const char* ssid = "Your_SSID";
const char* password = "Your_Password";
```
- `ssid`: The name of the Wi-Fi network to connect to.
- `password`: The password of the Wi-Fi network.

## Server and WebSocket Objects
```cpp
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
```
- `server`: An asynchronous web server object that listens on port 80.
- `webSocket`: A WebSocket server object that listens on port 81.

## Functions

### Processor Function
```cpp
String processor(const String& var){
  if(var == "STATE"){
    return "0";
  }
  return String();
}
```
- `processor`: A function that processes placeholders in HTML files. In this example, it replaces the placeholder "STATE" with "0".

### Not Found Handler
```cpp
void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
```
- `notFound`: A function that handles requests to undefined routes, returning a 404 error.

### WebSocket Message Handler
```cpp
void handleWebSocketMessage(uint8_t num, uint8_t *data, size_t len) {
  data[len] = 0; // Null-terminate the string
  Serial.printf("WebSocket Message: %s\n", (char *)data);

  // Send a response back to the client
  webSocket.sendTXT(num, "Message received");
}
```
- `handleWebSocketMessage`: A function that handles incoming WebSocket messages and sends a response back to the client.

### WebSocket Event Handler
```cpp
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from %s\n", num, ip.toString().c_str());
      }
      break;
    case WStype_TEXT:
      handleWebSocketMessage(num, payload, length);
      break;
  }
}
```
- `onWebSocketEvent`: A function that handles WebSocket events such as connections, disconnections, and incoming text messages.

## Setup Function
```cpp
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Route to load JavaScript file
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/script.js", "text/javascript");
  });

  // Route to load minified JavaScript file
  server.on("/script.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/script.min.js", "text/javascript");
  });

  // Start server
  server.begin();

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}
```
- `setup`: Initializes serial communication, mounts the LittleFS file system, connects to Wi-Fi, sets up web server routes, and starts the web and WebSocket servers.

## Loop Function
```cpp
void loop(){
  webSocket.loop();
}
```
- `loop`: Continuously handles WebSocket events.

This code sets up a basic web server and WebSocket server on an ESP8266. It serves files from the LittleFS file system and handles WebSocket connections, allowing for real-time communication between the client and the server.
