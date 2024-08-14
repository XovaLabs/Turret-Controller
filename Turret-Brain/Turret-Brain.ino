#include <AccelStepper.h>
#include <MultiStepper.h>

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

class WebServerManager {
public:
    WebServerManager(const char* ssid, const char* password)
    : ssid(ssid), password(password), server(80), webSocket(81) {}

    void begin() {
        // Serial port for debugging purposes
        Serial.begin(115200);

        // Initialize LittleFS
        if (!LittleFS.begin()) {
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
        Serial.println(WiFi.localIP());

        // Set up server routes
        setupServerRoutes();

        // Start server
        server.begin();

        // Start WebSocket server
        webSocket.begin();
        webSocket.onEvent(std::bind(&WebServerManager::onWebSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void loop() {
        webSocket.loop();
    }

private:
    const char* ssid;
    const char* password;
    AsyncWebServer server;
    WebSocketsServer webSocket;

    void setupServerRoutes() {
        // Route for root / web page
        server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/jerk.html", String(), false, std::bind(&WebServerManager::processor, this, std::placeholders::_1));
        });

        // Route to load JavaScript file
        server.on("/joy.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/joy.js", "text/javascript");
        });

        // Route to load minified JavaScript file
        server.on("/joy.min.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/joy.min.js", "text/javascript");
        });

        server.onNotFound(std::bind(&WebServerManager::notFound, this, std::placeholders::_1));
    }

    String processor(const String& var) {
        if (var == "STATE") {
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

  DynamicJsonDocument jsonDoc(2048);
  deserializeJson(jsonDoc, (char *)data);

  JsonObject joy1 = jsonDoc["joy1"];
  int xPosition1 = joy1["xPosition"].as<int>();
  int yPosition1 = joy1["yPosition"].as<int>();
  String direction1 = joy1["direction"].as<String>();
  int x1 = joy1["x"].as<int>();
  int y1 = joy1["y"].as<int>();

  JsonObject joy2 = jsonDoc["joy2"];
  int xPosition2 = joy2["xPosition"].as<int>();
  int yPosition2 = joy2["yPosition"].as<int>();
  String direction2 = joy2["direction"].as<String>();
  int x2 = joy2["x"].as<int>();
  int y2 = joy2["y"].as<int>();

  Serial.printf("Joy1: xPosition=%d, yPosition=%d, direction=%s, x=%d, y=%d\n", xPosition1, yPosition1, direction1.c_str(), x1, y1);
  Serial.printf("Joy2: xPosition=%d, yPosition=%d, direction=%s, x=%d, y=%d\n", xPosition2, yPosition2, direction2.c_str(), x2, y2);

  // Send a response back to the client
  webSocket.sendTXT(num, "Message received");
}

    void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        switch (type) {
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
};

WebServerManager serverManager("Rodriguez", "-HB9,bkCwR}FzXVp");

void setup() {
    serverManager.begin();
}

void loop() {
    serverManager.loop();
}
