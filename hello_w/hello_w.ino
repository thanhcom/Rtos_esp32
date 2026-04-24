#include "SensorModule.h"
#include "DhtSensorModule.h"
#include "CommModule.h"
#include "DataModels.h"
#include "ConfigManager.h"
#include "MqttModule.h" // Nhớ include module MQTT của bạn
#include "WifiModule.h"
#include "RfModule.h"

// --- Khởi tạo đối tượng và biến toàn cục ---
SensorModule mySensor;
DhtSensorModule myDht(4);
CommModule myComm;
ConfigManager myConfig;
MqttModule myMqtt;
WifiModule myWifi("KINH MAT HA NOI CS3", "Motnam6868");
RfModule myRf;

// Kho lưu trữ dữ liệu mới nhất để các Task dùng chung
SystemState g_state = {0, 0, 0}; 
QueueHandle_t sensorQueue;
QueueHandle_t rfQueue;

// --- Task 1: Gửi dữ liệu Voltage ---
void TaskSensor(void *pvParameters) {
    mySensor.begin();
    for (;;) {
        float v = mySensor.readData();
        g_state.volt = v; // Cập nhật kho chung

        SensorData data = {RANDOM_TEMP, v};
        xQueueSend(sensorQueue, &data, pdMS_TO_TICKS(10));
        vTaskDelay(pdMS_TO_TICKS(2000)); 
    }
}

// --- Task 2: Task DHT ---
void TaskDHT(void *pvParameters) {
    myDht.begin();
    for (;;) {
        float t = myDht.readTemperature();
        float h = myDht.readHumidity();

        if (!isnan(t) && !isnan(h)) {
            // 1. Cập nhật vào kho chung g_state
            g_state.temp = t;
            g_state.hum = h;

            // 2. Kiểm tra ngưỡng từ Config
            float threshold = myConfig.getTempThreshold();
            if (t > threshold) {
                Serial.printf("\n[ALERT] Nhiệt độ %.2f > %.2f!\n", t, threshold);
            }

            // 3. Gửi vào Queue cho TaskComm (hiển thị CLI)
            SensorData tData = {DHT_TEMP, t};
            SensorData hData = {DHT_HUM, h};
            xQueueSend(sensorQueue, &tData, pdMS_TO_TICKS(10));
            xQueueSend(sensorQueue, &hData, pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(2000)); // Đọc 2s/lần cho mượt CLI
    }
}

// --- Task 3: Task Giao tiếp (CLI Output) ---
void TaskComm(void *pvParameters) {
    SensorData received;
    for (;;) {
        if (xQueueReceive(sensorQueue, &received, portMAX_DELAY)) {
            String label = "";
            switch (received.type) {
                case RANDOM_TEMP: label = "Voltage"; break;
                case DHT_TEMP:    label = "DHT_Temp"; break;
                case DHT_HUM:     label = "DHT_Hum"; break;
            }
            myComm.sendToCLI(label, received.value);
        }
    }
}

// --- Task 4: CLI Input ---
void TaskCLI(void *pvParameters) {
    for (;;) {
        if (Serial.available()) {
            String input = Serial.readStringUntil('\n');
            input.trim();
            if (input.startsWith("SET_TEMP=")) {
                float newLimit = input.substring(9).toFloat();
                myConfig.setTempThreshold(newLimit);
                Serial.printf(">> Đã cập nhật ngưỡng: %.2f\n", newLimit);
            } 
            else if (input == "GET_CONFIG") {
                Serial.printf(">> Ngưỡng hiện tại: %.2f\n", myConfig.getTempThreshold());
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// --- Task 5: MQTT Publish & Listen ---
void TaskMQTT(void *pvParameters) {
    myMqtt.setup();
    
    // Biến lưu thời điểm gửi dữ liệu lần cuối
    unsigned long lastPublish = 0;

    for (;;) {
        // 1. Kiểm tra WiFi
        if (!myWifi.isConnected()) {
            myWifi.connect();
        }

        // 2. Nếu có WiFi
        if (myWifi.isConnected()) {
            if (myMqtt.connect()) {
                // QUAN TRỌNG: Gọi loop liên tục để check lệnh ON/OFF
                myMqtt.loop(); 
                
                // 3. Kiểm tra xem đã đến lúc gửi dữ liệu chưa (10 giây)
                if (millis() - lastPublish >= 10000) {
                    myMqtt.publishData(g_state.temp, g_state.hum);
                    lastPublish = millis(); // Cập nhật lại thời gian gửi
                    Serial.println("[MQTT] Đã gửi dữ liệu định kỳ");
                }
            } else {
                // Nếu lỗi kết nối MQTT thì đợi 5s rồi thử lại
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue; // Nhảy về đầu vòng lặp
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        // Đợi một khoảng rất ngắn (100ms) để nhường CPU 
        // nhưng vẫn đảm bảo phản hồi lệnh MQTT cực nhanh
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}
// --- Task 6: RF Communication ---
void TaskRF(void *pvParameters) {
    myRf.begin();

    for (;;) {
        if (myRf.available()) {
            String msg = myRf.receive();

            RFData rfData;
            memset(rfData.data, 0, sizeof(rfData.data));
            msg.toCharArray(rfData.data, sizeof(rfData.data));

            xQueueSend(rfQueue, &rfData, pdMS_TO_TICKS(10));

            Serial.println("[RF] Received: " + msg);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
// --- Task 7: Xử lý dữ liệu RF ---
void TaskRFProcess(void *pvParameters) {
    RFData rfData;

    for (;;) {
        if (xQueueReceive(rfQueue, &rfData, portMAX_DELAY)) {

            String msg = String(rfData.data);

            // 👇 xử lý tuỳ bạn
            Serial.println("[RF_PROCESS] " + msg);

            // ví dụ:
            if (msg == "ON") {
                digitalWrite(2, HIGH);
            } else if (msg == "OFF") {
                digitalWrite(2, LOW);
            }

            // hoặc parse dạng số
            float val = atof(rfData.data);
            if (val != 0) {
                Serial.printf("[RF_VALUE] %.2f\n", val);
            }
        }
    }
}

void setup() {
    // Khởi tạo Serial trước để debug
    Serial.begin(115200);

    pinMode(2, OUTPUT);
    digitalWrite(2, LOW); // Mặc định tắt

    myWifi.connect();
    myConfig.begin();
    sensorQueue = xQueueCreate(20, sizeof(SensorData));
    rfQueue = xQueueCreate(10, sizeof(RFData));
    if (sensorQueue != NULL) {
        xTaskCreate(TaskSensor, "TaskVoltage", 3072, NULL, 1, NULL);
        xTaskCreate(TaskDHT,    "TaskDHT",     4096, NULL, 1, NULL); 
        xTaskCreate(TaskComm,   "TaskComm",    3072, NULL, 2, NULL);
        xTaskCreate(TaskCLI,    "TaskCLI",     4096, NULL, 1, NULL);
        // MQTT cần Stack lớn vì chạy thư viện mạng phức tạp
        xTaskCreate(TaskMQTT,   "TaskMQTT",    8192, NULL, 1, NULL);
        xTaskCreate(TaskRF,        "TaskRF",        4096, NULL, 1, NULL);
        xTaskCreate(TaskRFProcess, "TaskRFProcess", 4096, NULL, 1, NULL);
    }
}

void loop() {}