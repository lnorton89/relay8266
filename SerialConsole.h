#ifndef SERIAL_CONSOLE_H
#define SERIAL_CONSOLE_H

#include <Arduino.h>

namespace SerialConsole {
    extern const char *colors[];
    const int colorsSize = 9;
    void appendToSerialBuffer(const String &message);
    String getSerialBuffer();
}

#endif