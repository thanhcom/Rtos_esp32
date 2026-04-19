#ifndef SENSOR_MODULE_H
#define SENSOR_MODULE_H

#include <Arduino.h>

class SensorModule {
public:
    void begin() {
        // Khởi tạo bộ sinh số ngẫu nhiên dựa trên nhiễu ở chân analog trống
        randomSeed(analogRead(0)); 
    }

    float readData() {
        // Giả lập đọc cảm biến nhiệt độ từ 20.0 đến 35.0 độ C
        // random(min, max) trả về số nguyên, nên ta nhân chia để lấy số thập phân
        float temp = random(2000, 3500) / 100.0; 
        return temp;
    }
};

#endif