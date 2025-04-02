#ifndef WEB_SERVER_HANDLERS_H
#define WEB_SERVER_HANDLERS_H

#include <ESP8266WebServer.h>

namespace WebServerHandlers {
    void setupHandlers(ESP8266WebServer& server);
    String getHTML();
}

#endif