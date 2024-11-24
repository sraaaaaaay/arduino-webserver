/** 
 * A basic web server which will gradually be extended
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <stdlib.h>
#include <config.h>

// Taken from config.h for privacy reasons (I don't want to publish my WiFi credentials on GitHub)
const char *SSID = SSID;
const char *PASSWORD = PASSWORD;

// Create server on port80, declare pin & status
AsyncWebServer server(80);
uint8_t LED_PIN = 2;
bool LED_status = LOW;


/**
 * Builds and returns a simple HTML page to be returned to a user
 * 
 * @param led1stat bool representing ESP32 LED status
 * @return pointer to HTML String
 */
String SendHTML(uint8_t led1stat) {

  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>LED Control</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr += ".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr += ".button-on {background-color: #3498db;}\n";
  ptr += ".button-on:active {background-color: #2980b9;}\n";
  ptr += ".button-off {background-color: #34495e;}\n";
  ptr += ".button-off:active {background-color: #2c3e50;}\n";
  ptr += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP32 Web Server</h1>\n";

  if (led1stat) {
    ptr += "<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led_off\">OFF</a>\n";
  } else {
    ptr += "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led_on\">ON</a>\n";
  }

  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}

/**
 * Sets up serial, pin mode, GET route handlers
 */
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Trying to connect to wifi...");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(200, "text/html", SendHTML(false));
  });

  server.on("/led_on", HTTP_GET, [](AsyncWebServerRequest *req) {
    LED_status = HIGH;
    req->send(200, "text/html", SendHTML(true));
  });

  server.on("/led_off", HTTP_GET, [](AsyncWebServerRequest *req) {
    LED_status = LOW;
    req->send(200, "text/html", SendHTML(false));
  });

  server.begin();
  Serial.println("HTTP server started");
}

/**
 * Handles wifi reconnection & delays
 */
void loop() {
  static unsigned long lastToggleTime = 0;
  unsigned long current = millis();

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(1000);
  }

  if (current - lastToggleTime >= 100) {
    lastToggleTime = currentMillis;

    if (LED_status) {
      digitalWrite(LED_PIN, HIGH);
    } else {
      digitalWrite(LED_PIN, LOW);
    }
  }
}