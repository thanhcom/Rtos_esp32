#ifndef DATA_MODELS_H
#define DATA_MODELS_H

enum SensorType { RANDOM_TEMP, DHT_TEMP, DHT_HUM };

struct SensorData {
    SensorType type;
    float value;
};

// --- Thêm vào ---
struct SystemState {
    float temp;
    float hum;
    float volt;
};
#endif