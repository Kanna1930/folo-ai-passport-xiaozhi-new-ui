#pragma once
#include "driver/i2c_master.h"
inline int64_t esp_timer_get_time() { return fake::now; }
