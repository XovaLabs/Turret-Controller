#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>

// Replace with your network credentials
const char* ssid = "Rodriguez";
const char* password = "-HB9,bkCwR}FzXVp";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Replaces placeholder with LED state value
String processor(const String& var){
  if(var == "STATE"){
    return "0";
  }
  return String();
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void handleWebSocketMessage(uint8_t num, uint8_t *data, size_t len) {
  data[len] = 0; // Null-terminate the string
  Serial.printf("WebSocket Message: %s\n", (char *)data);

  // Send a response back to the client
  webSocket.sendTXT(num, "Message received");
}

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
    request->send(LittleFS, "/jerk.html", String(), false, processor);
  });

  // Route to load JavaScript file
  server.on("/joy.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/joy.js", "text/javascript");
  });

  // Route to load minified JavaScript file
  server.on("/joy.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/joy.min.js", "text/javascript");
  });

  // Start server
  server.begin();

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop(){
  webSocket.loop();
}
