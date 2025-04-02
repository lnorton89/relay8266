#include "WebServerHandlers.h"
#include "RelayController.h"
#include "PinStatus.h"
#include "SerialConsole.h"
#include "TimeUtils.h"

namespace WebServerHandlers {
    String getHTML() {
        return "<!DOCTYPE HTML><html><head><title>Relay Control</title>"
               "<meta name='viewport' content='width=device-width, initial-scale=1'>"
               "<style>body { background-color: #121212; color: #ffffff; font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; }"
               ".container { display: flex; justify-content: space-evenly; flex-direction: row; align-items: flex-start; flex-wrap: wrap; }"
               ".card { background: #1e1e1e; padding: 20px; margin: 10px; width: 80%; max-width: 300px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); }"
               ".serial-console { width: 100%; max-width: 770px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.2); }"
               ".button { display: inline-block; padding: 10px 20px; margin: 10px; border: none; border-radius: 5px; color: white; text-decoration: none; font-size: 16px; font-weight: bold; cursor: pointer; }"
               ".button:disabled, .button:hover:disabled { background: #9e9e9e !important; cursor: not-allowed; }"
               ".button.turn-on:hover { background: #adf7b69e; }"
               ".button.turn-off:hover { background: #ff5e6e9e; }"
               ".turn-on { background: #adf7b6; color: #222; }"
               ".turn-off { background: #ff5e6e; }"
               ".console { background: #1e1e1e; margin: 0; width: 100%; text-align: left; height: 300px; overflow-y: auto; white-space: pre-wrap; font-family: monospace; scroll-behavior: smooth; }</style>"
               "</head><body><div class='container'>"
               "<div class='card'><h2>Relay 1</h2><p>Status: <span id='relay1_status'>" + String(RelayController::getRelayState(D0) == RelayController::OFF ? "OFF" : "ON") + "</span></p>"
               "<button onclick=\"toggleRelay(1, 'on')\" id='relay1_on' class='button turn-on'" + String(RelayController::getRelayState(D0) == RelayController::ON ? " disabled" : "") + ">Turn ON</button> "
               "<button onclick=\"toggleRelay(1, 'off')\" id='relay1_off' class='button turn-off'" + String(RelayController::getRelayState(D0) == RelayController::OFF ? " disabled" : "") + ">Turn OFF</button></div>"
               "<div class='card'><h2>Relay 2</h2><p>Status: <span id='relay2_status'>" + String(RelayController::getRelayState(D1) == RelayController::OFF ? "OFF" : "ON") + "</span></p>"
               "<button onclick=\"toggleRelay(2, 'on')\" id='relay2_on' class='button turn-on'" + String(RelayController::getRelayState(D1) == RelayController::ON ? " disabled" : "") + ">Turn ON</button> "
               "<button onclick=\"toggleRelay(2, 'off')\" id='relay2_off' class='button turn-off'" + String(RelayController::getRelayState(D1) == RelayController::OFF ? " disabled" : "") + ">Turn OFF</button></div>"
               "<div class='card serial-console'><h2>Serial Console</h2><div class='console' id='console'></div>" + PinStatus::getPinTableHTML(16) +
               "<script>setInterval(() => { fetch('/serial').then(res => res.text()).then(text => { document.getElementById('console').innerHTML = text; document.getElementById('console').scrollTop = document.getElementById('console').scrollHeight; }); }, 800);"
               "function toggleRelay(relay, action) { fetch('/relay' + relay + '/' + action).then(() => updateStatus()); }"
               "function updateStatus() { fetch('/status').then(res => res.json()).then(data => {"
               "document.getElementById('relay1_status').innerText = data.relay1 ? 'OFF' : 'ON';"
               "document.getElementById('relay2_status').innerText = data.relay2 ? 'OFF' : 'ON';"
               "document.getElementById('relay1_on').disabled = data.relay1 == 0;"
               "document.getElementById('relay1_off').disabled = data.relay1 == 1;"
               "document.getElementById('relay2_on').disabled = data.relay2 == 0;"
               "document.getElementById('relay2_off').disabled = data.relay2 == 1;"
               "}); }"
               "setInterval(() => { fetch('/pinstatus').then(res => res.json()).then(data => {"
               "data.forEach(pin => { document.getElementById('pin' + pin.pin).style.backgroundColor = pin.color; document.getElementById('pin' + pin.pin).innerText = 'D' + pin.pin; });"
               "}); }, 500);</script></div></div></body></html>";
    }

 void setupHandlers(ESP8266WebServer& server) {
        server.on("/", HTTP_GET, [&server]() { server.send(200, "text/html", getHTML()); });
        server.on("/status", HTTP_GET, [&server]() { server.send(200, "application/json", "{\"relay1\":" + String(RelayController::getRelayState(D0)) + ",\"relay2\":" + String(RelayController::getRelayState(D1)) + "}"); });
        server.on("/serial", HTTP_GET, [&server]() { server.send(200, "text/html", SerialConsole::getSerialBuffer()); });
        server.on("/pinstatus", HTTP_GET, [&server]() { server.send(200, "application/json", PinStatus::getPinStatus(16)); });
        server.on("/relay1/on", HTTP_GET, [&server]() { RelayController::setRelayState(D0, RelayController::ON); SerialConsole::appendToSerialBuffer("Relay 1 turned ON"); server.send(200, "text/html", getHTML()); });
        server.on("/relay1/off", HTTP_GET, [&server]() { RelayController::setRelayState(D0, RelayController::OFF); SerialConsole::appendToSerialBuffer("Relay 1 turned OFF"); server.send(200, "text/html", getHTML()); });
        server.on("/relay2/on", HTTP_GET, [&server]() { RelayController::setRelayState(D1, RelayController::ON); SerialConsole::appendToSerialBuffer("Relay 2 turned ON"); server.send(200, "text/html", getHTML()); });
        server.on("/relay2/off", HTTP_GET, [&server]() { RelayController::setRelayState(D1, RelayController::OFF); SerialConsole::appendToSerialBuffer("Relay 2 turned OFF"); server.send(200, "text/html", getHTML()); });
    }
}