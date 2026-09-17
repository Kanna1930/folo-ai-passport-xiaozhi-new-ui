#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>

constexpr int ESP_OK = 0;
constexpr int I2C_ADDR_BIT_LEN_7 = 7;
using i2c_master_bus_handle_t = void*;
using i2c_master_dev_handle_t = void*;
struct i2c_device_config_t {
    int dev_addr_length = 0;
    unsigned device_address = 0;
    unsigned scl_speed_hz = 0;
};
namespace fake {
inline int64_t now = 0;
inline int error = 0;
inline unsigned address = 0;
inline uint8_t version = 0xA0, mode = 0, soc = 50;
inline unsigned voltage = 12000;
inline int reads = 0, removed = 0;
inline std::vector<std::pair<uint8_t, uint8_t>> writes;
inline void Reset() {
    now = 0; error = 0; address = 0; version = 0xA0; mode = 0; soc = 50;
    voltage = 12000; reads = 0; removed = 0; writes.clear();
}
}
inline int i2c_master_probe(i2c_master_bus_handle_t, unsigned address, int) {
    fake::address = address;
    return fake::error;
}
inline int i2c_master_bus_add_device(i2c_master_bus_handle_t, const i2c_device_config_t* config,
                                    i2c_master_dev_handle_t* device) {
    fake::address = config->device_address;
    *device = reinterpret_cast<void*>(1);
    return fake::error;
}
inline int i2c_master_bus_rm_device(i2c_master_dev_handle_t) { ++fake::removed; return 0; }
inline int i2c_master_transmit_receive(i2c_master_dev_handle_t, const uint8_t* reg, size_t,
                                     uint8_t* out, size_t size, int) {
    ++fake::reads;
    if (fake::error) return fake::error;
    if (*reg == 0x00 && size == 1) *out = fake::version;
    else if (*reg == 0x08 && size == 1) *out = fake::mode;
    else if (*reg == 0x02 && size == 4) {
        out[0] = fake::voltage >> 8; out[1] = fake::voltage & 0xFF;
        out[2] = fake::soc; out[3] = 0;
    } else return -1;
    return 0;
}
inline int i2c_master_transmit(i2c_master_dev_handle_t, const uint8_t* data, size_t size, int) {
    if (size != 2) return -1;
    fake::writes.emplace_back(data[0], data[1]);
    return fake::error;
}
