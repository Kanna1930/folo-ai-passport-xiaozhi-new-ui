#pragma once

#include <cstdint>
#include "device_state.h"

// Monotonic time only: clock synchronization must not change the sleep deadline.
class ScreenIdle {
public:
    static constexpr int64_t kTimeoutUs = 180LL * 1000 * 1000;

    bool Activity(int64_t now) {
        const bool was_asleep = asleep_;
        last_activity_ = now;
        asleep_ = false;
        return was_asleep;
    }

    bool Tick(int64_t now, DeviceState state, bool voice_active) {
        const bool can_sleep = state == kDeviceStateIdle || state == kDeviceStateListening ||
                               state == kDeviceStateConnecting;
        if (!can_sleep || voice_active) {
            Activity(now);
        } else if (now - last_activity_ >= kTimeoutUs) {
            asleep_ = true;
        }
        return asleep_;
    }

    bool asleep() const { return asleep_; }

private:
    int64_t last_activity_ = 0;
    bool asleep_ = false;
};
