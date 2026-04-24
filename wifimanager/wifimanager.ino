#include "WifiManagerModule.h"
#include "ConfigManager.h"
#include "MqttModule.h"
#include "DataModels.h"

WifiManagerModule wifi;
MqttModule mqtt;
ConfigManager config;


SystemState state;

// ================= SENSOR TASK =================
void sensorTask(void *pv) {
    Serial.println("[Sensor] Task start");

    while (1) {
        // giả lập dữ liệu
        state.temp = random(200, 350) / 10.0;
        state.hum  = random(400, 800) / 10.0;

        Serial.printf("[Sensor] Temp=%.1f Hum=%.1f\n", state.temp, state.hum);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

// ================= MQTT TASK =================
void mqttTask(void *pv) {
    Serial.println("[MQTT] Task start");

    TickType_t lastPublish = 0;

    while (1) {
        if (wifi.isConnected()) {
            if (mqtt.connect()) {

                mqtt.loop(); // 👈 chạy liên tục

                // publish mỗi 10s
                if (xTaskGetTickCount() - lastPublish > 10000 / portTICK_PERIOD_MS) {
                    float temp = state.temp;
                    float hum  = state.hum;

                    mqtt.publishData(temp, hum);
                    lastPublish = xTaskGetTickCount();
                }
            }
        }

        vTaskDelay(100 / portTICK_PERIOD_MS); // 👈 cực quan trọng
    }
}
// ================= SETUP =================
void setup() {
    Serial.begin(115200);
    //reset(config);
    pinMode(2, OUTPUT);

    config.begin();

    wifi.begin(config);   // nhập WiFi + MQTT config
    mqtt.setup(config);   // 👈 CHỈ gọi 1 lần ở đây

    xTaskCreatePinnedToCore(
        sensorTask,
        "sensorTask",
        3000,
        NULL,
        1,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        mqttTask,
        "mqttTask",
        5000,
        NULL,
        1,
        NULL,
        0
    );
}

void loop() {
    wifi.loop(); // giữ reconnect WiFi
}


void reset(ConfigManager &config) {
    WiFiManager wm;

    wm.resetSettings(); // xóa WiFi
    config.clear();     // xóa MQTT

    Serial.println("[System] Full reset...");
    ESP.restart();
}