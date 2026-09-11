# Folo AI Passport ESP32-C3

Pure XiaoZhi firmware target for the Folo AI Passport badge. This build replaces the former badge/NES application and does not share its runtime data partitions.

## Hardware

- ESP32-C3, 8 MB flash, no PSRAM
- 240 x 320 ST7789P3 display over SPI2
- ES8311 codec with full-duplex I2S
- Three ADC ladder keys on GPIO0
- CW2017 battery gauge on the shared I2C bus (0x63)
- Native USB Serial/JTAG on GPIO18/GPIO19

## Buttons

- Up: volume +10
- Down: volume -10
- Confirm: start/stop a conversation; during startup it enters Wi-Fi configuration

## Build

```sh
python scripts/build.py folo/ai-passport-c3 --name folo-ai-passport-c3 --language zh-CN
```

The release uses `partitions/v2/8m.csv` and produces a merged image under `build/`. Flashing this image replaces the existing partition table. Preserve any rollback image separately before flashing.

## Battery display modification

This local variant enables the CW2017 battery gauge using the reference 520mAh battery profile from `FoloToy/ai-passport` and displays the SOC percentage next to the existing battery icon in the LVGL status bar. The hardware has no dedicated charge-status input in this board definition, so the status bar reports SOC only; it does not show a charging bolt.

The profile is written only when the CW2017 profile does not match the reference profile. On first boot after flashing, initialization may take a few seconds while the gauge becomes ready.
