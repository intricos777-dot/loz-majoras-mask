"""Smoke tests for LOZ Majora's Mask build."""
from pathlib import Path

ROOT = Path('/home/sin/Projects/loz-majoras-mask')
BIN = ROOT / 'build' / 'mm'

def test_mm_binary_exists():
    assert BIN.exists(), 'mm binary missing'

if __name__ == '__main__':
    test_mm_binary_exists()
    print('[OK] mm smoke test passed')
