# Folo AI Passport - Pixel Sister UI v1

## Added
- 176x176 pixel-style assistant portraits: neutral / listening / thinking / speaking
- Portrait displayed at about 188x188 on the 240x320 LCD
- idle -> neutral, listening -> listening, connecting -> thinking, speaking/notifying -> speaking
- CW2017 battery percentage remains enabled
- LCD backlight turns off after 180 seconds in idle state
- Button actions, wake word, state changes and notifications restore saved brightness
- Wi-Fi and wake-word detection stay alive while the LCD is dark (not deep sleep)

## Build
Use the included GitHub Actions workflow or:

```bash
python scripts/build.py folo/ai-passport-c3 --name folo-ai-passport-c3 --language zh-CN --wake-word nihaoxiaozhi
```

Expected merged output: `build/merged-binary.bin`.

## Validation note
This package is structurally checked here, but a complete ESP-IDF compile/link is not possible in this runtime because ESP-IDF is not installed. Use GitHub Actions for the definitive compile check.
