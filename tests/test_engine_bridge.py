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

    def test_self_heal_script_executable(self):
        import os
        path = REPO / "scripts/self-heal.sh"
        self.assertTrue(path.exists())
        self.assertTrue(os.access(path, os.X_OK))


if __name__ == "__main__":
    unittest.main()
