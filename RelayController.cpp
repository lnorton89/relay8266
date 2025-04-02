#include "RelayController.h"

namespace RelayController {
    RelayState relay1State = OFF;
    RelayState relay2State = OFF;

    int relay1Pin;
    int relay2Pin;

    void setupRelays(int relay1Pin, int relay2Pin) {
        RelayController::relay1Pin = relay1Pin;
        RelayController::relay2Pin = relay2Pin;
        pinMode(relay1Pin, OUTPUT);
        pinMode(relay2Pin, OUTPUT);
        digitalWrite(relay1Pin, relay1State);
        digitalWrite(relay2Pin, relay2State);
    }

    void setRelayState(int relayPin, RelayState state) {
        digitalWrite(relayPin, state);
        if (relayPin == relay1Pin) {
            relay1State = state;
        } else if (relayPin == relay2Pin) {
            relay2State = state;
        }
    }

    RelayState getRelayState(int relayPin) {
        if (relayPin == relay1Pin) {
            return relay1State;
        } else if (relayPin == relay2Pin) {
            return relay2State;
        }
        return OFF; // Default
    }
}