#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>

namespace RelayController {
    enum RelayState { OFF = HIGH, ON = LOW };

    extern RelayState relay1State;
    extern RelayState relay2State;

    void setupRelays(int relay1Pin, int relay2Pin);
    void setRelayState(int relayPin, RelayState state);
    RelayState getRelayState(int relayPin);
}

#endif