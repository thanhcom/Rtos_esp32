#ifndef DATA_MODELS_H
#define DATA_MODELS_H

#include <Arduino.h>

enum SensorType { 
    RANDOM_TEMP, 
    DHT_TEMP, 
    DHT_HUM
};


struct SensorData {
    SensorType type;
    float value;
};

// 👇 RF dùng struct riêng
struct RFData {
    char data[64];
};

struct SystemState {
    float temp;
    float hum;
    float volt;
};

#endif