#include "PinStatus.h"

namespace PinStatus {
    String getPinStatus(int digitalPinCount) {
        String pinStatus = "[";
        for (int pin = 0; pin < digitalPinCount; pin++) {
            int actualPin = getActualPin(pin);
            int pinState = digitalRead(actualPin);
            String pinColor = (pinState == HIGH) ? "#adf7b6" : "#ff5e6e";
            pinStatus += "{\"pin\":" + String(pin) + ",\"state\":" + String(pinState) + ",\"color\":\"" + pinColor + "\"},";
        }
        pinStatus.remove(pinStatus.length() - 1);
        return pinStatus + "]";
    }

    String getPinTableHTML(int digitalPinCount) {
        String html = "<style> @media (min-width: 768px) { .pin-container { flex-wrap: nowrap; } } </style>"
                      "<div class='pin-container' style='display: flex; justify-content: center; flex-wrap: wrap; margin-top: 10px;'>";
        for (int pin = 0; pin < digitalPinCount; pin++) {
            html += "<div style='text-align: center; margin: 2px; padding: 2px; border: 1px solid #333; border-radius: 3px; min-width: 30px;'>"
                    "<span id='pin" + String(pin) + "' style='background-color:#ff5e6e; font-weight: bold; color: #333; padding: 3px; width: 30px; display: inline-block;'></span>"
                    "</div>";
        }
        return html + "</div>";
    }

    int getActualPin(int pin) {
        switch (pin) {
            case 0:  return D0;
            case 1:  return D1;
            case 2:  return D2;
            case 3:  return D3;
            case 4:  return D4;
            case 5:  return D5;
            case 6:  return D6;
            case 7:  return D7;
            case 8:  return D8;
            case 9:  return D9;
            case 10: return D10;
            default: return pin;
        }
    }
}