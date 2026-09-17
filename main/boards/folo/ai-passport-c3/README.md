# Folo AI Passport ESP32-C3

Pure XiaoZhi firmware target for the Folo AI Passport badge. This build replaces the former badge/NES application and does not share its runtime data partitions.

## Hardware

- ESP32-C3, 8 MB flash, no PSRAM
- 240 x 320 ST7789P3 display over SPI2
- ES8311 codec with full-duplex I2S
- Three ADC ladder keys on GPIO0
- Native USB Serial/JTAG on GPIO18/GPIO19

## Buttons

- Up: volume +10
- Down: volume -10
- Confirm: start/stop a conversation; during startup it enters Wi-Fi configuration
- Long confirm while idle: settings; up/down select, confirm opens an item
- While asleep: the first key only wakes the screen

## Pixel UI and Battery

The board uses `FoloDisplay`, a 48x48 code-drawn portrait displayed at 192x192.
The UI includes volume, brightness, device information, and confirmed Wi-Fi/restart actions.
After 180 seconds without activity in idle, listening, or connecting, the backlight turns off.
Speaking, provisioning, activation, and upgrades keep the screen on. Audio and wake-word
detection are not disabled by screen-off. The animation timer pauses while asleep.

CW2017 on the codec I2C bus reports percentage, cached for 15 seconds. Missing/invalid
readings show `--%`. No undocumented battery profile or charging-detection GPIO is used.
Charging/discharging flags remain unknown until a real charger-status signal is identified.

This modification has host tests, not an ESP-IDF build or physical hardware validation.
See `docs/FOLO_UI_GUIDE.md` for the current verification checklist.

## Build

```sh
python scripts/build.py folo/ai-passport-c3 --name folo-ai-passport-c3 --language zh-CN
```

The release uses `partitions/v2/8m.csv` and produces a merged image under `build/`. Flashing this image replaces the existing partition table. Preserve any rollback image separately before flashing.
