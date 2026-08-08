#!/bin/bash
# Self-heal wrapper
set -uo pipefail
LOG="logs/self-heal.log"
mkdir -p "$(dirname "$LOG")"
ts() { date '+%Y-%m-%d %H:%M:%S'; }
echo "[$(ts)] self-heal start" >> "$LOG"
for d in scripts configs tests docs security; do
  [ -d "$d" ] || mkdir -p "$d" && echo "[$(ts)] created $d" >> "$LOG"
done
echo "[$(ts)] self-heal end" >> "$LOG"
