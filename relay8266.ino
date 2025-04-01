#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "WifiCredentials.h"

#ifndef SSID
#define SSID "wifissid"
#define PASSWORD "pass"
#endif

#define RELAY1_PIN D0
#define RELAY2_PIN D1
#define DIGITAL_PIN_COUNT 16  // For example, we'll show 16 digital pins

const char *ssid = SSID;
const char *password = PASSWORD;

int relay1_state = HIGH;
int relay2_state = HIGH;

String serialBuffer = "";
const char* colors[] = {"ffa8a8","ffd4a8","fff1a8","a8ffa8","a8e2ff","d4a8ff"};
int colorIndex = 0;

WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", -18000, 60000); // UTC-6 (Central Standard Time) with 60-second update interval

ESP8266WebServer server(80);

String getTimestamp() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  time_t rawTime = (time_t)epochTime;

  struct tm *tm = localtime(&rawTime);

  int hours = tm->tm_hour;
  int minutes = tm->tm_min;
  int seconds = tm->tm_sec;

  // Convert to 12-hour format and determine AM/PM
  String ampm = "AM";
  if (hours >= 12) {
    ampm = "PM";
    if (hours > 12) {
      hours -= 12;  // Convert hour to 12-hour format
    }
  } else if (hours == 0) {
    hours = 12;  // Midnight is 12 AM
  }

  // Add leading zeros if needed
  String timestamp = String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds) + " " + ampm;

  return timestamp;
}

void appendToSerialBuffer(String message) {
  String timestamp = "<span style='color: #" + String(colors[colorIndex]) + ";'>" + getTimestamp() + "</span>";
  serialBuffer += timestamp + " - " + message + "<br>"; // Use <br> for HTML line breaks
  colorIndex = (colorIndex + 1) % 6; // cycle through colors array.
  if (serialBuffer.length() > 2000) { // Increase buffer size for HTML
    serialBuffer = serialBuffer.substring(serialBuffer.length() - 2000);
  }
}

String getPinStatus() {
  String pinStatus = "";
  for (int pin = 0; pin < DIGITAL_PIN_COUNT; pin++) {
    int actualPin = -1;

    // Map pin number to the correct Dxx (NodeMCU) pin names
    switch (pin) {
      case 0: actualPin = D0; break;    // D0 (GPIO16)
      case 1: actualPin = D1; break;    // D1 (GPIO5)
      case 2: actualPin = D2; break;    // D2 (GPIO4)
      case 3: actualPin = D3; break;    // D3 (GPIO0)
      case 4: actualPin = D4; break;    // D4 (GPIO2)
      case 5: actualPin = D5; break;    // D5 (GPIO14)
      case 6: actualPin = D6; break;    // D6 (GPIO12)
      case 7: actualPin = D7; break;    // D7 (GPIO13)
      case 8: actualPin = D8; break;    // D8 (GPIO15)
      case 9: actualPin = D9; break;    // RX (GPIO3)
      case 10: actualPin = D10; break;  // TX (GPIO1)
      default: actualPin = pin; break;  // For other pins if needed
    }

    // Read the pin state
    int pinState = digitalRead(actualPin);
    String pinColor = (pinState == HIGH) ? "#adf7b6" : "#ff5e6e";
    pinStatus += "{\"pin\":" + String(pin) + ",\"state\":\"" + (pinState == HIGH ? "HIGH" : "LOW") + "\",\"color\":\"" + pinColor + "\"},";
  }
  pinStatus.remove(pinStatus.length() - 1);  // Remove last comma
  return "[" + pinStatus + "]";
}

String getPinTableHTML() {
  // Pin mapping for ESP8266 GPIO to Dxx pins
  String pinNames[] = { "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "D10", "D11", "D12", "D13", "D14", "D15" };

  String html = "<table style='width:100%; text-align:center; border-collapse: collapse; margin-top: 20px;'>";
  html += "<tr><th>Pin (D)</th><th>Status</th></tr>";

  for (int pin = 0; pin < DIGITAL_PIN_COUNT; pin++) {
    html += "<tr><td style='padding: 5px;'>" + pinNames[pin] + "</td>";
    html += "<td id='pin" + String(pin) + "' style='background-color:#ff5e6e; font-weight: bold; color: #333; padding: 5px; width: 30px;'></td></tr>";
  }

  html += "</table>";
  return html;
}

String getHTML() {
  String html = "<!DOCTYPE HTML>";
  html += "<html><head><title>Relay Control</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { background-color: #121212; color: #ffffff; font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; }";
  html += ".container { display: flex; justify-content: space-evenly; flex-direction: row; align-items: flex-start; flex-wrap: wrap; }";
  html += ".card { background: #1e1e1e; padding: 20px; margin: 10px; width: 80%; max-width: 300px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); }";
  html += ".serial-console { width: 100%; max-width: 718px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); }";
  html += ".button { display: inline-block; padding: 10px 20px; margin: 10px; border: none; border-radius: 5px; color: white; text-decoration: none; font-size: 16px; font-weight: bold; cursor: pointer; }";
  html += ".button:disabled, .button:hover:disabled { background: #9e9e9e !important; cursor: not-allowed; }";
  html += ".button.turn-on:hover { background: #adf7b69e; }";
  html += ".button.turn-off:hover { background: #ff5e6e9e; }";
  html += ".turn-on { background: #adf7b6; color: #222; }";
  html += ".turn-off { background: #ff5e6e; }";
  html += ".console { background: #1e1e1e; margin: 0; width: 100%; text-align: left; height: 300px; overflow-y: auto; white-space: pre-wrap; font-family: monospace; scroll-behavior: smooth; }";
  html += "</style></head><body>";

  html += "<div class='container'>";
  html += "<div class='card'><h2>Relay 1</h2><p>Status: <span id='relay1_status'>" + String(relay1_state == HIGH ? "OFF" : "ON") + "</span></p>";
  html += "<button onclick=\"toggleRelay(1, 'on')\" id='relay1_on' class='button turn-on'" + String(relay1_state == LOW ? " disabled" : "") + ">Turn ON</button> ";
  html += "<button onclick=\"toggleRelay(1, 'off')\" id='relay1_off' class='button turn-off'" + String(relay1_state == HIGH ? " disabled" : "") + ">Turn OFF</button></div>";

  html += "<div class='card'><h2>Relay 2</h2><p>Status: <span id='relay2_status'>" + String(relay2_state == HIGH ? "OFF" : "ON") + "</span></p>";
  html += "<button onclick=\"toggleRelay(2, 'on')\" id='relay2_on' class='button turn-on'" + String(relay2_state == LOW ? " disabled" : "") + ">Turn ON</button> ";
  html += "<button onclick=\"toggleRelay(2, 'off')\" id='relay2_off' class='button turn-off'" + String(relay2_state == HIGH ? " disabled" : "") + ">Turn OFF</button></div>";

  html += "<div class='card serial-console'><h2>Serial Console</h2>";
  html += "<div class='console' id='console'></div>";
  html += "<script>";
  html += "setInterval(() => { fetch('/serial').then(res => res.text()).then(text => { document.getElementById('console').innerHTML = text; document.getElementById('console').scrollTop = document.getElementById('console').scrollHeight; }); }, 800);"; // Use innerHTML for HTML content
  html += "function toggleRelay(relay, action) { fetch('/relay' + relay + '/' + action).then(() => updateStatus()); }";
  html += "function updateStatus() { fetch('/status').then(res => res.json()).then(data => {";
  html += "document.getElementById('relay1_status').innerText = data.relay1 ? 'OFF' : 'ON';";
  html += "document.getElementById('relay2_status').innerText = data.relay2 ? 'OFF' : 'ON';";
  html += "document.getElementById('relay1_on').disabled = data.relay1 == 0;";
  html += "document.getElementById('relay1_off').disabled = data.relay1 == 1;";
  html += "document.getElementById('relay2_on').disabled = data.relay2 == 0;";
  html += "document.getElementById('relay2_off').disabled = data.relay2 == 1;";
  html += "}); }";

  html += "setInterval(() => { fetch('/pinstatus').then(res => res.json()).then(data => {";
  html += "data.forEach(pin => {";
  html += "  document.getElementById('pin' + pin.pin).style.backgroundColor = pin.color;";
  html += "  document.getElementById('pin' + pin.pin).innerText = pin.state;";
  html += "});";
  html += "}); }, 500);";  // Update every 500 ms
  html += "</script></div>";

  html += "<div class='card'><h2>Digital Pins</h2>";
  html += getPinTableHTML();
  html += "</div>";

  html += "</div></body></html>";
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

  // Attempting to sync with NTP server
  Serial.println("Attempting to sync with NTP server...");
  appendToSerialBuffer("Attempting to sync with NTP server...");

  timeClient.begin();  // Start NTP client

  Serial.println("Time sync success!");
  appendToSerialBuffer("Time sync success!");

  // Print current time and timezone settings
  String timestamp = getTimestamp();  // Get the current time
  Serial.print("Current time: ");
  Serial.println(timestamp);
  appendToSerialBuffer("Current time: " + timestamp);

  // Print timezone settings
  Serial.println("Timezone: UTC-6 (Central Standard Time)");
  appendToSerialBuffer("Timezone: UTC-6 (Central Standard Time)");

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", getHTML());
  });

  server.on("/status", HTTP_GET, []() {
    String json = "{\"relay1\":" + String(relay1_state) + ",\"relay2\":" + String(relay2_state) + "}";
    server.send(200, "application/json", json);
  });

  server.on("/serial", HTTP_GET, []() {
    server.send(200, "text/html", serialBuffer); // send as html
  });

  server.on("/pinstatus", HTTP_GET, []() {
    String status = getPinStatus();
    server.send(200, "application/json", status);
  });

  server.on("/relay1/on", HTTP_GET, []() {
    relay1_state = LOW;
    digitalWrite(RELAY1_PIN, relay1_state);
    appendToSerialBuffer("Relay 1 turned ON");
    server.send(200, "text/html", getHTML());
  });
  server.on("/relay1/off", HTTP_GET, []() {
    relay1_state = HIGH;
    digitalWrite(RELAY1_PIN, relay1_state);
    appendToSerialBuffer("Relay 1 turned OFF");
    server.send(200, "text/html", getHTML());
  });

  server.on("/relay2/on", HTTP_GET, []() {
    relay2_state = LOW;
    digitalWrite(RELAY2_PIN, relay2_state);
    appendToSerialBuffer("Relay 2 turned ON");
    server.send(200, "text/html", getHTML());
  });
  server.on("/relay2/off", HTTP_GET, []() {
    relay2_state = HIGH;
    digitalWrite(RELAY2_PIN, relay2_state);
    appendToSerialBuffer("Relay 2 turned OFF");
    server.send(200, "text/html", getHTML());
  });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();
}