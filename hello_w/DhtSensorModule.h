#ifndef DHT_SENSOR_MODULE_H
#define DHT_SENSOR_MODULE_H

#include <Arduino.h>
class DhtSensorModule {
private:
    uint8_t _pin;

public:
    // Constructor vẫn giữ nguyên chân pin để sau này lắp thật không phải sửa code main
    DhtSensorModule(uint8_t pin) : _pin(pin) {}

    void begin() {
        // Khởi tạo hạt giống ngẫu nhiên dựa trên nhiễu ở chân analog trống
        randomSeed(analogRead(0)); 
        Serial.println("DHT11 Simulator Started (No hardware needed)");
    }

    float readTemperature() {
        // DHT11 thường đo từ 0-50 độ C
        // Giả lập giá trị từ 24.5 đến 32.0 độ C
        float t = random(2450, 3200) / 100.0;
        return t;
    }

    float readHumidity() {
        // DHT11 thường đo từ 20-90%
        // Giả lập giá trị từ 45.0% đến 75.0%
        float h = random(4500, 7500) / 100.0;
        return h;
    }
};

#endif
