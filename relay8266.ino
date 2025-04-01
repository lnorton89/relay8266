#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "WifiCredentials.h"

#ifndef SSID
  #define SSID "wifissid"
  #define PASSWORD "pass"
#endif

#define RELAY1_PIN D0  // The ESP8266 pin connected to Relay 1
#define RELAY2_PIN D1  // The ESP8266 pin connected to Relay 2

const char *ssid = SSID;
const char *password = PASSWORD;  

ESP8266WebServer server(80); // Web server on port 80

int relay1_state = LOW;
int relay2_state = LOW;

String getHTML() {
  String html = "<!DOCTYPE HTML>";
  html += "<html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { background-color: #121212; color: #ffffff; font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; }";
  html += ".card { background: #1e1e1e; padding: 20px; margin: 10px auto; width: 80%; max-width: 300px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); }";
  html += ".button { display: inline-block; padding: 10px 20px; margin: 10px; border: none; border-radius: 5px; background: #6200ea; color: white; text-decoration: none; font-size: 16px; cursor: pointer; }";
  html += ".button:hover { background: #3700b3; }";
  html += "</style></head><body>";
  html += "<div class='card'><h2>Relay 1</h2><p>Status: ";
  html += (relay1_state == HIGH) ? "OFF" : "ON";
  html += "</p><a href='/relay1/on' class='button'>Turn ON</a> <a href='/relay1/off' class='button'>Turn OFF</a></div>";
  html += "<div class='card'><h2>Relay 2</h2><p>Status: ";
  html += (relay2_state == HIGH) ? "OFF" : "ON";
  html += "</p><a href='/relay2/on' class='button'>Turn ON</a> <a href='/relay2/off' class='button'>Turn OFF</a></div>";
  html += "</body></html>";
  return html;
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, relay1_state);
  digitalWrite(RELAY2_PIN, relay2_state);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP8266 Web Server's IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getHTML());
  });

  server.on("/relay1/on", HTTP_GET, []() {
    relay1_state = LOW;
    digitalWrite(RELAY1_PIN, relay1_state);
    server.send(200, "text/html", getHTML());
  });
  server.on("/relay1/off", HTTP_GET, []() {
    relay1_state = HIGH;
    digitalWrite(RELAY1_PIN, relay1_state);
    server.send(200, "text/html", getHTML());
  });

  server.on("/relay2/on", HTTP_GET, []() {
    relay2_state = LOW;
    digitalWrite(RELAY2_PIN, relay2_state);
    server.send(200, "text/html", getHTML());
  });
  server.on("/relay2/off", HTTP_GET, []() {
    relay2_state = HIGH;
    digitalWrite(RELAY2_PIN, relay2_state);
    server.send(200, "text/html", getHTML());
  });

  server.begin();

  // OTA Setup
  ArduinoOTA.setHostname("ESP8266-OTA");
  ArduinoOTA.onStart([]() {
    Serial.println("OTA Update Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA Update End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("OTA Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}
