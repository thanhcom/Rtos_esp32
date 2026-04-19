#ifndef COMM_MODULE_H
#define COMM_MODULE_H

#include <Arduino.h>

class CommModule {
public:
    void begin(long baudrate) {
        Serial.begin(baudrate);
    }

    void sendToCLI(String label, float value) {
        Serial.print("[" + label + "]: ");
        Serial.println(value);
    }
};

#endif
