#ifndef PIN_STATUS_H
#define PIN_STATUS_H

#include <Arduino.h>

namespace PinStatus {
    String getPinStatus(int digitalPinCount);
    String getPinTableHTML(int digitalPinCount);
    int getActualPin(int pin);
}

#endif