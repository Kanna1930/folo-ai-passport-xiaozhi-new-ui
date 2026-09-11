import pathlib
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]

class FoloNewUiTests(unittest.TestCase):
    def test_pixel_assets_exist_and_are_small(self):
        asset_dir = ROOT / "main/assets/folo-pixel-sister"
        for name in ("neutral", "listening", "thinking", "speaking"):
            p = asset_dir / f"{name}.png"
            self.assertTrue(p.exists(), p)
            self.assertLess(p.stat().st_size, 64 * 1024)

    def test_three_minute_screen_timeout(self):
        h = (ROOT / "main/application.h").read_text()
        cc = (ROOT / "main/application.cc").read_text()
        self.assertIn("kScreenOffTimeoutSeconds = 180", h)
        self.assertIn("backlight->SetBrightness(0)", cc)
        self.assertIn("backlight->RestoreBrightness()", cc)

    def test_state_portraits(self):
        cc = (ROOT / "main/application.cc").read_text()
        for name in ("thinking", "listening", "speaking"):
            self.assertIn(f'SetEmotion("{name}")', cc)

    def test_custom_collection_is_packaged(self):
        cmake = (ROOT / "main/CMakeLists.txt").read_text()
        script = (ROOT / "scripts/build_default_assets.py").read_text()
        self.assertIn("DEFAULT_EMOJI_COLLECTION folo-pixel-sister", cmake)
        self.assertIn("main', 'assets', 'folo-pixel-sister", script)

if __name__ == "__main__":
    unittest.main()
