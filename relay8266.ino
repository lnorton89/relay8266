#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "RelayController.h"
#include "PinStatus.h"
#include "WebServerHandlers.h"
#include "SerialConsole.h"
#include "TimeUtils.h"

#include "WifiCredentials.h"

#ifndef SSID
#define SSID "wifissid"
#define PASSWORD "pass"
#endif

#define RELAY1_PIN D0
#define RELAY2_PIN D1
#define DIGITAL_PIN_COUNT 16

const char *ssid = SSID;
const char *password = PASSWORD;

ESP8266WebServer server(80);

void setup() {
    Serial.begin(9600);

    RelayController::setupRelays(RELAY1_PIN, RELAY2_PIN);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    TimeUtils::setupNTP();

    WebServerHandlers::setupHandlers(server);

    server.begin();
    Serial.println("Server started");
}

void loop() {
    server.handleClient();
    ArduinoOTA.handle();
}