#include "TimeUtils.h"
#include "SerialConsole.h" // Include SerialConsole.h

namespace TimeUtils {
    WiFiUDP udp;
    NTPClient timeClient(udp, "pool.ntp.org", -18000, 60000);

    void setupNTP() {
        Serial.println("Attempting to sync with NTP server...");
        SerialConsole::appendToSerialBuffer("Attempting to sync with NTP server...");
        timeClient.begin();
        Serial.println("Time sync success!");
        SerialConsole::appendToSerialBuffer("Time sync success!");
        String timestamp = getTimestamp();
        Serial.print("Current time: ");
        Serial.println(timestamp);
        SerialConsole::appendToSerialBuffer("Current time: " + timestamp);
        Serial.println("Timezone: UTC-6 (Central Standard Time)");
        SerialConsole::appendToSerialBuffer("Timezone: UTC-6 (Central Standard Time)");
    }

    String getTimestamp() {
        timeClient.update();
        time_t rawTime = timeClient.getEpochTime();
        struct tm *timeInfo = localtime(&rawTime);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%I:%M:%S %p", timeInfo);
        return String(timestamp);
    }
}