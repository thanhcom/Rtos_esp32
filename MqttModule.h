#include <WiFi.h>
#include <PubSubClient.h>
#include "DataModels.h"

class MqttModule {
private:
    WiFiClient espClient;
    PubSubClient client;
    
    // Thông tin server bạn cung cấp
    const char* mqtt_server = "192.168.1.66";
    const int   mqtt_port   = 1882;
    const char* mqtt_user   = "thanhcom";
    const char* mqtt_pass   = "laodaicaha";
    const char* clientID    = "ESP32C3_ThanhTrang";
    
    // Topic mới theo yêu cầu
    const char* topic_pub   = "testTopic";
    const char* topic_sub = "testTopic/control"; // Topic nhận lệnh
public:
    MqttModule() : client(espClient) {}
    // Hàm callback để xử lý tin nhắn đến
    static void callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("\n[MQTT] Lệnh mới từ topic: ");
        Serial.println(topic);

        String message;
        for (int i = 0; i < length; i++) message += (char)payload[i];
        Serial.println("[MQTT] Nội dung: " + message);

        // Gửi nội dung lệnh vào một Queue điều khiển hoặc xử lý trực tiếp
        // Ở đây mình sẽ dùng một biến toàn cục đơn giản cho bẹn dễ hình dung
        if (message == "ON") digitalWrite(2, HIGH);
        else if (message == "OFF") digitalWrite(2, LOW);
    }

    void setup() {
        client.setServer(mqtt_server, mqtt_port);
        // Tăng buffer để xử lý chuỗi JSON an toàn
        client.setBufferSize(512,512); 
        // Thiết lập hàm callback
        client.setCallback(callback);
    }

    bool connect() {
        if (!client.connected()) {
            Serial.printf("[MQTT] Đang kết nối tới %s...\n", mqtt_server);

            // Kết nối với ID, User và Password
            if (client.connect(clientID, mqtt_user, mqtt_pass)) {
                Serial.println("[MQTT] Đã kết nối thành công!");
                client.subscribe(topic_sub);
            } else {
                Serial.printf("[MQTT] Thất bại, rc=%d\n", client.state());
            }
        }
        return client.connected();
    }

    void publishData(float temp, float hum) {
        if (client.connected()) {
            char msg[64];
            // Đóng gói JSON: {"temp":25.5,"hum":60.0}
            snprintf(msg, sizeof(msg), "{\"temp\":%.1f,\"hum\":%.1f}", temp, hum);
            
            if (client.publish(topic_pub, msg)) {
                Serial.printf("[MQTT] Đã gửi tới [%s]: %s\n", topic_pub, msg);
            } else {
                Serial.println("[MQTT] Gửi tin nhắn thất bại!");
            }
        }
    }

    void loop() {
        client.loop(); // Duy trì kết nối và xử lý gói tin đến
    }
};