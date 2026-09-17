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


if __name__ == "__main__":
    unittest.main()
