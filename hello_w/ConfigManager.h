#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Preferences.h>

class ConfigManager {
private:
    Preferences _prefs;

public:
    // Khởi tạo và mở "không gian" lưu trữ tên là "app-config"
    void begin() {
        _prefs.begin("app-config", false); // false = Read/Write
    }

    // Lưu ngưỡng nhiệt độ cảnh báo
    void setTempThreshold(float threshold) {
        _prefs.putFloat("temp_limit", threshold);
    }

    // Đọc ngưỡng nhiệt độ (mặc định là 30.0 nếu chưa có)
    float getTempThreshold() {
        return _prefs.getFloat("temp_limit", 30.0);
    }

    // Lưu trạng thái thiết bị (ví dụ: bật/tắt tự động)
    void setAutoMode(bool enable) {
        _prefs.putBool("auto_mode", enable);
    }

    bool getAutoMode() {
        return _prefs.getBool("auto_mode", true);
    }
};

#endif
