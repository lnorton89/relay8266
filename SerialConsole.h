#ifndef SERIAL_CONSOLE_H
#define SERIAL_CONSOLE_H

#include <Arduino.h>

namespace SerialConsole {
    void appendToSerialBuffer(const String &message);
    String getSerialBuffer();
}

#endif