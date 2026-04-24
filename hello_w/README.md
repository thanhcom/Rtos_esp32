# ESP32 FreeRTOS - Hệ Thống Giám Sát Cảm Biến Đa Nhiệm (IoT Framework)

Dự án này là một framework cơ bản nhưng mạnh mẽ dành cho ESP32, sử dụng hệ điều hành thời gian thực **FreeRTOS** để quản lý các tác vụ song song. Hệ thống thực hiện thu thập dữ liệu từ nhiều cảm biến, quản lý cấu hình qua bộ nhớ NVS và cung cấp giao diện dòng lệnh (CLI) để tương tác trực tiếp.

## 1. Kiến Trúc Hệ Thống (Software Architecture)

Hệ thống được thiết kế theo mô hình phân lớp (Layered Architecture) để đảm bảo tính module hóa và dễ dàng mở rộng:

* **Driver Layer:** Chứa các Module điều khiển phần cứng (`SensorModule`, `DhtSensorModule`).
* **Service Layer (Kernel):** Sử dụng FreeRTOS Scheduler để điều phối các Task.
* **Communication Layer:** Quản lý giao tiếp Serial và hàng đợi dữ liệu (`Queue`).
* **Storage Layer:** Quản lý cấu hình không bay hơi thông qua `ConfigManager` (NVS).



---

## 2. Các Tính Năng Chính

### 🚀 Đa nhiệm thời gian thực (Multitasking)
Sử dụng 4 Task chạy song song với các chu kỳ khác nhau:
- **TaskSensor (2s/lần):** Đọc giá trị điện áp giả lập.
- **TaskDHT (5s/lần):** Đọc nhiệt độ và độ ẩm từ cảm biến DHT11.
- **TaskComm (Ưu tiên cao):** Xử lý luồng in dữ liệu ra Serial một cách mượt mà.
- **TaskCLI:** Lắng nghe lệnh từ người dùng để cấu hình hệ thống "nóng".

### 📦 Giao tiếp an toàn qua Queue
Dữ liệu từ các task cảm biến được đóng gói thành một `struct SensorData` và gửi vào hàng đợi chung. Điều này giúp tránh xung đột tài nguyên (Race Condition) khi nhiều tác vụ muốn truy cập Serial cùng lúc.

### 💾 Quản lý cấu hình (Config Manager)
Tích hợp lớp `ConfigManager` sử dụng thư viện **Preferences**. Các thông số cấu hình (như ngưỡng cảnh báo nhiệt độ) sẽ được lưu vào bộ nhớ Flash của ESP32 và không bị mất đi khi khởi động lại.

---

## 3. Danh Sách Module & Chức Năng

| Module | File | Chức năng chính |
| :--- | :--- | :--- |
| **Kernel** | `main.ino` | Khởi tạo hệ thống, tạo Task và Queue. Điều phối logic chính. |
| **Sensor** | `SensorModule.h` | Đọc dữ liệu từ cảm biến Analog/Voltage. |
| **DHT** | `DhtSensorModule.h` | Giao tiếp với cảm biến DHT11/DHT22. |
| **Comm** | `CommModule.h` | Định dạng dữ liệu và xuất ra giao diện CLI. |
| **Config** | `ConfigManager.h` | Lưu/Đọc thông số từ bộ nhớ Flash (NVS). |
| **Models** | `DataModels.h` | Định nghĩa kiểu dữ liệu dùng chung (`struct`, `enum`). |

---

## 4. Hướng Dẫn Cấu Hình & Vận Hành

### Cách nạp code
1. Cài đặt board ESP32 trong Arduino IDE.
2. Cài đặt thư viện `DHT sensor library` và `Adafruit Unified Sensor`.
3. Mở Serial Monitor với Baudrate: `115200`.

### Tương tác qua Serial CLI
Bạn có thể điều khiển thiết bị trực tiếp bằng cách gõ các lệnh sau vào Serial Monitor:

1.  **Thay đổi ngưỡng cảnh báo nhiệt độ:**
    * Cú pháp: `SET_TEMP=[giá_trị]`
    * Ví dụ: `SET_TEMP=35.5`
    * *Chức năng:* Hệ thống sẽ lưu số 35.5 vào Flash. Nếu nhiệt độ DHT vượt mức này, hệ thống sẽ báo `[ALERT]`.

2.  **Xem cấu hình hiện tại:**
    * Cú pháp: `GET_CONFIG`
    * *Chức năng:* Trả về các thông số đang được lưu trong bộ nhớ.

---

## 5. Lưu Ý Về Bộ Nhớ (Stack Size)

Do hệ thống sử dụng nhiều hàm xử lý chuỗi (`String`, `printf`) và truy cập Flash, kích thước Stack cấp cho mỗi Task cần được lưu ý để tránh lỗi **Stack Overflow**:

- **TaskDHT/TaskCLI:** Khuyến nghị ít nhất `4096` bytes.
- **TaskSensor/TaskComm:** Khuyến nghị ít nhất `3072` bytes.

*Nếu gặp lỗi `Stack canary watchpoint triggered`, hãy tăng giá trị Stack trong hàm `xTaskCreate`.*

---
**Người thực hiện:** [Tên của bạn/Thành Trang Electric]
**Phiên bản:** 1.1 (FreeRTOS Core)
