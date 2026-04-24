#ifndef MQTT_MODULE_H
#define MQTT_MODULE_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "DataModels.h"
#include "ConfigManager.h"

class MqttModule {
private:
    WiFiClient espClient;
    PubSubClient client;
    ConfigManager *_config;

    // biến runtime (không hardcode nữa)
    String mqtt_server;
    int    mqtt_port;
    String mqtt_user;
    String mqtt_pass;
    String clientID;

    const char* topic_pub = "testTopic";
    const char* topic_sub = "testTopic/control";

public:
    MqttModule() : client(espClient) {}

    // 👇 thêm config vào đây
    void setup(ConfigManager &config) {
        _config = &config;

        // đọc từ Preferences
        mqtt_server = _config->getString("mqtt_server", "192.168.1.66");
        mqtt_port   = _config->getString("mqtt_port", "1882").toInt();
        mqtt_user   = _config->getString("mqtt_user", "");
        mqtt_pass   = _config->getString("mqtt_pass", "");
        clientID    = _config->getString("client_id", "ESP32");

        client.setServer(mqtt_server.c_str(), mqtt_port);
        client.setBufferSize(512, 512);
        client.setCallback(callback);

        Serial.println("[MQTT] Config loaded");
    }

    static void callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("\n[MQTT] Topic: ");
        Serial.println(topic);

        String message;
        for (int i = 0; i < length; i++) message += (char)payload[i];

        Serial.println("[MQTT] Msg: " + message);

        if (message == "ON") digitalWrite(2, HIGH);
        else if (message == "OFF") digitalWrite(2, LOW);
    }

    bool connect() {
        if (!client.connected()) {
            Serial.printf("[MQTT] Connecting to %s:%d...\n", mqtt_server.c_str(), mqtt_port);

            if (client.connect(
                    clientID.c_str(),
                    mqtt_user.c_str(),
                    mqtt_pass.c_str()
                )) {

                Serial.println("[MQTT] Connected!");
                client.subscribe(topic_sub);

            } else {
                Serial.printf("[MQTT] Failed, rc=%d\n", client.state());
            }
        }
        return client.connected();
    }

    void publishData(float temp, float hum) {
        if (client.connected()) {
            char msg[64];
            snprintf(msg, sizeof(msg),
                     "{\"temp\":%.1f,\"hum\":%.1f}", temp, hum);

            if (client.publish(topic_pub, msg)) {
                Serial.printf("[MQTT] Pub [%s]: %s\n", topic_pub, msg);
            } else {
                Serial.println("[MQTT] Publish fail!");
            }
        }
    }

    void loop() {
        client.loop();
    }
};

#endif