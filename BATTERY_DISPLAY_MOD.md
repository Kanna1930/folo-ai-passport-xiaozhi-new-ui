# CW2017 battery display modification

Changes in this variant:

1. Implemented `FoloAiPassportC3Board::GetBatteryLevel()` using the CW2017 at I2C address `0x63`.
2. Added the FoloToy AI Passport 520mAh CW2017 battery profile from the official `FoloToy/ai-passport` BSP reference.
3. Added a numeric SOC label (`0..100%`) beside the existing battery icon.
4. Kept the existing XiaoZhi UI, audio, buttons, networking and display behavior unchanged.

## Important

The board definition does not expose a dedicated charger-status GPIO, so this modification intentionally reports the battery percentage only. The existing battery icon is still used, but a charging-bolt state cannot be confirmed from the available hardware interface.

Build with the project's normal command:

```sh
python scripts/build.py folo/ai-passport-c3 --name folo-ai-passport-c3 --language zh-CN --wake-word nihaoxiaozhi
```

The modified source was not compiled in this environment because ESP-IDF is not installed here. Test the generated firmware on a physical AI Passport before normal use.
