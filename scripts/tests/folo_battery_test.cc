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
        assert(fake::writes.empty());
        assert(!gauge.GetLevel(level));
        fake::now = 1000000;
        assert(gauge.GetLevel(level) && level == 50);
        int reads = fake::reads;
        fake::now += 1000000;
        assert(gauge.GetLevel(level) && fake::reads == reads);
        fake::now += 15000000;
        fake::soc = 0;
        assert(gauge.GetLevel(level) && level == 0);
        fake::now += 15000000;
        fake::soc = 101;
        assert(!gauge.GetLevel(level));
        fake::now += 15000000;
        fake::soc = 100;
        assert(gauge.GetLevel(level) && level == 100);
        fake::now += 15000000;
        fake::voltage = 0;
        assert(!gauge.GetLevel(level));
        fake::now += 15000000;
        fake::voltage = 12000;
        fake::error = -1;
        assert(!gauge.GetLevel(level));
        fake::now += 15000000;
        fake::error = 0;
        assert(gauge.GetLevel(level));
    }
    assert(fake::removed == 1);
    fake::Reset();
    {
        fake::mode = 0xF0;
        Cw2017 gauge;
        assert(gauge.Initialize(nullptr));
        const std::vector<std::pair<uint8_t, uint8_t>> expected = {{0x08, 0x30}, {0x08, 0x00}};
        assert(fake::writes == expected);
    }
    fake::Reset();
    {
        fake::version = 0xFF;
        Cw2017 gauge;
        assert(!gauge.Initialize(nullptr));
        assert(!gauge.GetLevel(level));
        assert(fake::writes.empty());
    }
    assert(fake::removed == 1);
    fake::Reset();
    {
        fake::error = -1;
        Cw2017 gauge;
        assert(!gauge.Initialize(nullptr));
        assert(!gauge.GetLevel(level));
    }
    std::cout << "Gauge address, identity, wake sequence, cache, 0/100%, invalid SOC, "
                 "invalid voltage, missing device, and I2C recovery passed.\n";
}
