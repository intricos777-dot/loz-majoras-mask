#!/bin/bash
# Self-debug wrapper
set -uo pipefail
LOG="logs/debug-latest.log"
mkdir -p "$(dirname "$LOG")"
ts() { date '+%Y-%m-%d %H:%M:%S'; }
echo "[$(ts)] self-debug start" >> "$LOG"
echo "[$(ts)] CMake scaffold check" >> "$LOG"
if [ -f "CMakeLists.txt" ]; then
  echo "[$(ts)] PASS CMakeLists.txt present" >> "$LOG"
else
  echo "[$(ts)] FAIL CMakeLists.txt missing" >> "$LOG"
fi
echo "[$(ts)] self-debug end" >> "$LOG"
