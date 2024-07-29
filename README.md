# Turret-Brain
## Template
```
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Your_SSID";
const char* password = "Your_Password";

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the built-in LED pin as an output
  pinMode(LED_BUILTIN, OUTPUT);

  // Define a route for handling POST requests
  server.on("/control", HTTP_POST, [](AsyncWebServerRequest *request){
    // Declare a temporary buffer for storing the JSON data
    const size_t capacity = JSON_OBJECT_SIZE(1) + 30;
    DynamicJsonDocument doc(capacity);
    
    // Read the request body
    if (request->hasParam("plain", true)) {
      String json = request->getParam("plain", true)->value();
      // Parse the JSON data
      DeserializationError error = deserializeJson(doc, json);
      
      if (!error) {
        String state = doc["state"];
        
        // Process the state parameter
        if (state == "on") {
          digitalWrite(LED_BUILTIN, HIGH); // Turn LED on
          request->send(200, "application/json", "{\"message\":\"LED is ON\"}");
        } else if (state == "off") {
          digitalWrite(LED_BUILTIN, LOW); // Turn LED off
          request->send(200, "application/json", "{\"message\":\"LED is OFF\"}");
        } else {
          request->send(400, "application/json", "{\"error\":\"Invalid state\"}");
        }
      } else {
        // If there's an error in parsing JSON
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      }
    } else {
      // If the request body is missing
      request->send(400, "application/json", "{\"error\":\"Missing body\"}");   
    }
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing to do here
}

```
## Sending a Request - Postman Example
```
curl -X POST http://<ESP32_IP>/control -H "Content-Type: application/json" -d '{"state":"on"}'
curl -X POST http://<ESP32_IP>/control -H "Content-Type: application/json" -d '{"state":"off"}'
```

## Template Edited - Website Example
```
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

// Variables to store the car's state
String carDirection = "stopped";
int carSpeed = 0;

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize GPIO pins if necessary
  // pinMode(somePin, OUTPUT);

  // Define a route for handling GET requests
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    // Create a JSON object to send the current state
    DynamicJsonDocument doc(1024);
    doc["direction"] = carDirection;
    doc["speed"] = carSpeed;

    String response;
    serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // Define a route for handling POST requests
  server.on("/control", HTTP_POST, [](AsyncWebServerRequest *request){
    // Declare a temporary buffer for storing the JSON data
    const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
    DynamicJsonDocument doc(capacity);
    
    // Read the request body
    if (request->hasParam("plain", true)) {
      String json = request->getParam("plain", true)->value();
      // Parse the JSON data
      DeserializationError error = deserializeJson(doc, json);
      
      if (!error) {
        if (doc.containsKey("direction")) {
          carDirection = doc["direction"].as<String>();
        }
        if (doc.containsKey("speed")) {
          carSpeed = doc["speed"];
        }
        
        // Process the direction and speed
        // For example, control the motors based on direction and speed

        request->send(200, "application/json", "{\"message\":\"Car state updated\"}");
      } else {
        // If there's an error in parsing JSON
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      }
    } else {
      // If the request body is missing
      request->send(400, "application/json", "{\"error\":\"Missing body\"}");
    }
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing to do here
}

```
### Example of controlling the car - Post request Structure
```
curl -X POST http://<ESP32_IP>/control -H "Content-Type: application/json" -d '{"direction":"forward","speed":50}'
curl -X POST http://<ESP32_IP>/control -H "Content-Type: application/json" -d '{"direction":"backward","speed":30}'

```
