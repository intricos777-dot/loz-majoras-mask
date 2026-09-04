"""Tests for LOZ Majora's Mask engine bridge."""
import unittest
from pathlib import Path

REPO = Path("/home/sin/Projects/loz-majoras-mask")


class TestMMEngineBridge(unittest.TestCase):
    def test_header_declares_namespace_and_class(self):
        text = (REPO / "src/engine_bridge.h").read_text()
        self.assertIn("namespace loz_mm", text)
        self.assertIn("class EngineBridge", text)

    def test_cpp_implements_bridge_methods(self):
        text = (REPO / "src/engine_bridge.cpp").read_text()
        self.assertIn("namespace loz_mm", text)
        self.assertIn("EngineBridge::init", text)
        self.assertIn("EngineBridge::run", text)
        self.assertIn("te::Engine::instance().initialize", text)

    def test_cmake_lists_references_sources(self):
        text = (REPO / "CMakeLists.txt").read_text()
        self.assertIn("loz-majoras-mask", text)
        self.assertIn("src/main.cpp", text)

    def test_readme_exists(self):
        self.assertTrue((REPO / "README.md").exists())

    def test_build_scripts_are_executable(self):
        import os
        self.assertTrue((REPO / "scripts").is_dir())
        for name in ("build-mm.sh", "setup-2ship.sh", "setup-2ship.ps1"):
            path = REPO / f"scripts/{name}"
            self.assertTrue(path.exists(), f"missing {name}")
        self.assertTrue(os.access(REPO / "scripts/build-mm.sh", os.X_OK))


if __name__ == "__main__":
    unittest.main()
