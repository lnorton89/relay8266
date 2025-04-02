#include "SerialConsole.h"
#include "TimeUtils.h"

namespace SerialConsole {
    String serialBuffer;
    const char *colors[] = {"ffadad", "ffd6a5", "fdffb6", "caffbf", "9bf6ff", "a0c4ff", "bdb2ff", "ffc6ff", "fffffc"};
    int colorIndex = 0;

    void appendToSerialBuffer(const String &message) {
        String timestamp = "<span style='color: #" + String(colors[colorIndex]) + ";'>" + TimeUtils::getTimestamp() + "</span>";
        String coloredMessage = message;

        coloredMessage.replace("ON", "<span style='color: #adf7b6;'>ON</span>");
        coloredMessage.replace("OFF", "<span style='color: #ff5e6e;'>OFF</span>");

        serialBuffer += timestamp + " - " + coloredMessage + "<br>";
        colorIndex = (colorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
        if (serialBuffer.length() > 2000) {
            serialBuffer = serialBuffer.substring(serialBuffer.length() - 2000);
        }
    }

    String getSerialBuffer() {
        return serialBuffer;
    }
}