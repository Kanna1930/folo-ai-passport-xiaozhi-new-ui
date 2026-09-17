#include "cw2017.h"
#include <cassert>
#include <iostream>

int main() {
    int level = -1;
    fake::Reset();
    {
        Cw2017 gauge;
        assert(gauge.Initialize(nullptr));
        assert(fake::address == 0x63);
        const std::vector<std::pair<uint8_t, uint8_t>> expected = {{0x08, 0x00}};
        assert(fake::writes == expected);
        assert(gauge.GetLevel(level) && level == 50);
        fake::soc = 0;
        assert(gauge.GetLevel(level) && level == 0);
        fake::soc = 101;
        assert(!gauge.GetLevel(level));
        fake::soc = 100;
        assert(gauge.GetLevel(level) && level == 100);
        // Upstream reads SOC directly; an unrelated voltage sample must not
        // suppress an otherwise valid percentage.
        fake::voltage = 0;
        assert(gauge.GetLevel(level) && level == 100);
        fake::error = -1;
        assert(!gauge.GetLevel(level));
        fake::error = 0;
        assert(gauge.GetLevel(level));
    }
    assert(fake::removed == 1);
    fake::Reset();
    {
        fake::version = 0xFF;
        Cw2017 gauge;
        assert(gauge.Initialize(nullptr));
        const std::vector<std::pair<uint8_t, uint8_t>> expected = {{0x08, 0x00}};
        assert(fake::writes == expected);
    }
    assert(fake::removed == 1);
    fake::Reset();
    {
        fake::error = -1;
        Cw2017 gauge;
        assert(!gauge.Initialize(nullptr));
        assert(!gauge.GetLevel(level));
    }
    std::cout << "Gauge address, compatible identity detection, normal-mode wake, 0/100%, "
                 "invalid SOC, voltage-independent SOC, missing device, and I2C recovery passed.\n";
}
