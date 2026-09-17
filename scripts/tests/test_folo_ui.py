import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BOARD = ROOT / "main/boards/folo/ai-passport-c3"
TESTS = ROOT / "scripts/tests"


class FoloUiHostTests(unittest.TestCase):
    def compile_and_run(self, filename):
        compiler = shutil.which("g++")
        self.assertIsNotNone(compiler, "g++ is required for the Folo UI host tests")
        with tempfile.TemporaryDirectory() as directory:
            executable = Path(directory) / "test"
            build = subprocess.run(
                [compiler, "-std=c++17", "-Wall", "-Wextra", "-Werror",
                 "-I", str(TESTS / "folo_stubs"), "-I", str(ROOT / "main"),
                 "-I", str(BOARD), str(TESTS / filename), "-o", str(executable)],
                capture_output=True, text=True,
            )
            self.assertEqual(build.returncode, 0, build.stdout + build.stderr)
            result = subprocess.run([str(executable)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_idle_policy_and_portrait_bounds(self):
        self.compile_and_run("folo_ui_test.cc")

    def test_cw2017_driver_with_simulated_i2c(self):
        self.compile_and_run("folo_battery_test.cc")

    def test_saved_brightness_integer_types_and_bounds(self):
        # Compile the actual source expressions with both int and long storage.
        # On the ESP32-C3 toolchain int32_t is long; on many hosts it is int.
        board = (BOARD / "folo_ai_passport_c3_board.cc").read_text()
        assignment = re.search(r'brightness_\s*=\s*[^;]*GetInt\("brightness"[^;]*;', board)
        self.assertIsNotNone(assignment)
        backlight = (ROOT / "main/boards/common/backlight.cc").read_text()
        restore = re.search(r"void Backlight::RestoreBrightness\(\) \{(.*?)\n\}",
                            backlight, re.DOTALL)
        self.assertIsNotNone(restore)
        source = '#include <algorithm>\n#include <cstdint>\n#include <cassert>\n'
        source += '#define ESP_LOGW(...) ((void)0)\n'
        for name, storage in (("host", "int"), ("target", "long")):
            source += f'''namespace {name} {{
                {storage} saved;
                struct Settings {{
                    Settings(const char*, bool = false) {{}}
                    {storage} GetInt(const char*, int) {{ return saved; }}
                }};
                int menu() {{
                    Settings settings("display", false);
                    int brightness_;
                    {assignment.group(0)}
                    return brightness_;
                }}
                uint8_t restored;
                void SetBrightness(uint8_t value) {{ restored = value; }}
                void restore() {{ {restore.group(1)} }}
            }}\n'''
        source += '''int main() {
            for (long value : {-2147483647L - 1, -1L, 0L, 1L, 9L, 10L, 75L,
                               100L, 255L, 256L, 300L, 2147483647L}) {
                int expected = value < 10 ? 10 : value > 100 ? 100 : int(value);
                host::saved = int(value);
                target::saved = value;
                assert(host::menu() == expected);
                assert(target::menu() == expected);
                host::restore(); target::restore();
                assert(host::restored == expected);
                assert(target::restored == expected);
            }
        }'''
        compiler = shutil.which("g++")
        self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory() as directory:
            executable = Path(directory) / "brightness"
            build = subprocess.run(
                [compiler, "-std=c++17", "-Wall", "-Wextra", "-Werror",
                 "-x", "c++", "-", "-o", str(executable)],
                input=source, capture_output=True, text=True,
            )
            self.assertEqual(build.returncode, 0, build.stderr)
            result = subprocess.run([str(executable)], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()
