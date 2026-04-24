#ifndef RF_MODULE_H
#define RF_MODULE_H

#include <Arduino.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>

// chú ý khi dùng nhiều task tranh chấp phải dùng kết hợp xSemaphoreTake(rfMutex, portMAX_DELAY); và xSemaphoreGive(rfMutex);
class RfModule {
private:
    float _freq;

public:
    // Constructor: cho phép set tần số (mặc định 433MHz)
    RfModule(float freq = 433.92) : _freq(freq) {}

    void begin() {
        ELECHOUSE_cc1101.Init();
        ELECHOUSE_cc1101.setMHZ(_freq);
        ELECHOUSE_cc1101.SetRx();

        Serial.println("RF Module Started (CC1101)");
    }

    bool available() {
        return ELECHOUSE_cc1101.CheckReceiveFlag();
    }

    String receive() {
        byte buffer[64] = {0};
        int len = ELECHOUSE_cc1101.ReceiveData(buffer);

        String data = "";
        for (int i = 0; i < len; i++) {
            data += (char)buffer[i];
        }

        ELECHOUSE_cc1101.SetRx(); // quay lại RX sau khi đọc
        return data;
    }

    void send(const String &data) {
        ELECHOUSE_cc1101.SetTx();
        ELECHOUSE_cc1101.SendData((byte*)data.c_str(), data.length());
        ELECHOUSE_cc1101.SetRx();

        Serial.println("[RF] Sent: " + data);
    }
};

#endif
