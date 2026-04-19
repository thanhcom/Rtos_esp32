#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include <WiFi.h>

class WifiModule {
private:
    const char* _ssid;
    const char* _password;

public:
    WifiModule(const char* ssid, const char* password) {
        _ssid = ssid;
        _password = password;
    }

    void connect() {
        if (WiFi.status() == WL_CONNECTED) return;

        Serial.println("\n[WiFi] Đang kết nối...");
        WiFi.begin(_ssid, _password);

        // Đợi kết nối (không dùng vòng lặp vô tận để tránh treo Task khác)
        int retry = 0;
        while (WiFi.status() != WL_CONNECTED && retry < 20) {
            delay(500);
            Serial.print(".");
            retry++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n[WiFi] Đã kết nối!");
            Serial.print("[WiFi] IP: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("\n[WiFi] Kết nối thất bại!");
        }
    }

    bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }
};

#endif
