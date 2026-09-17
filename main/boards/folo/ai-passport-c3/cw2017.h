#pragma once

#include <driver/i2c_master.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mutex>

class Cw2017 {
public:
    bool Initialize(i2c_master_bus_handle_t bus) {
        // CW2017 uses 0x63, unlike the CW2015's 0x62.
        if (i2c_master_probe(bus, 0x63, 50) != ESP_OK) return false;
        i2c_device_config_t config = {};
        config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
        config.device_address = 0x63;
        config.scl_speed_hz = 100000;
        if (i2c_master_bus_add_device(bus, &config, &device_) != ESP_OK) return false;
        uint8_t version = 0, mode = 0;
        if (!Read(0x00, &version, 1) || version != 0xA0 || !Read(0x08, &mode, 1)) {
            return Disable();
        }
        if (mode & 0xF0) {
            // Wake/restart only; never overwrite the battery-specific factory profile.
            if (!Write(0x08, 0x30)) return Disable();
            vTaskDelay(pdMS_TO_TICKS(20));
            if (!Write(0x08, 0x00)) return Disable();
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        next_read_ = esp_timer_get_time() + 1000000;
        return true;
    }

    ~Cw2017() { if (device_) i2c_master_bus_rm_device(device_); }

    bool GetLevel(int& level) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!device_) return false;
        const auto now = esp_timer_get_time();
        if (now >= next_read_) {
            next_read_ = now + 15000000;
            uint8_t data[4] = {};
            valid_ = Read(0x02, data, sizeof(data));
            if (valid_) {
                const unsigned voltage = (unsigned(data[0]) << 8) | data[1];
                // Reject impossible samples, not legitimate 0% readings.
                valid_ = voltage >= 6400 && voltage <= 16000 && data[2] <= 100;
                if (valid_) level_ = data[2];
            }
        }
        if (!valid_) return false;
        level = level_;
        return true;
    }

private:
    bool Read(uint8_t reg, uint8_t* out, size_t size) {
        return i2c_master_transmit_receive(device_, &reg, 1, out, size, 50) == ESP_OK;
    }
    bool Write(uint8_t reg, uint8_t value) {
        const uint8_t data[] = {reg, value};
        return i2c_master_transmit(device_, data, sizeof(data), 50) == ESP_OK;
    }
    bool Disable() {
        i2c_master_bus_rm_device(device_);
        device_ = nullptr;
        return false;
    }
    i2c_master_dev_handle_t device_ = nullptr;
    std::mutex mutex_;
    int64_t next_read_ = 0;
    int level_ = 0;
    bool valid_ = false;
};
