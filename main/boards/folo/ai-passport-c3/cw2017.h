#pragma once

#include <driver/i2c_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <mutex>

class Cw2017 {
public:
    bool Initialize(i2c_master_bus_handle_t bus) {
        i2c_device_config_t config = {};
        config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
        config.device_address = 0x63;
        config.scl_speed_hz = 100000;
        if (i2c_master_bus_add_device(bus, &config, &device_) != ESP_OK) {
            device_ = nullptr;
            return false;
        }

        // Different production batches may expose a different version byte.
        // A successful read is enough to identify the optional CW2017.
        uint8_t version = 0;
        if (!Read(0x00, &version, 1)) {
            return Disable();
        }

        // Match the upstream AI Passport driver: select normal operation and
        // keep the chip's built-in battery profile untouched.
        Write(0x08, 0x00);
        vTaskDelay(pdMS_TO_TICKS(100));
        return true;
    }

    ~Cw2017() { if (device_) i2c_master_bus_rm_device(device_); }

    bool GetLevel(int& level) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!device_) return false;
        uint8_t soc[2] = {};
        if (!Read(0x04, soc, sizeof(soc)) || soc[0] > 100) return false;
        level = soc[0];
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
};
