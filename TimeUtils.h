#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

namespace TimeUtils {
    void setupNTP();
    String getTimestamp();
}

#endif