#!/usr/bin/env bash
set -e

SKYRIM_DIR="/home/sin/.steam/steam/steamapps/common/Skyrim Special Edition"
COMPAT_DIR="/home/sin/.steam/steam/steamapps/compatdata/489830"
LOG_DIR="$COMPAT_DIR/pfx/drive_c/users/steamuser/Documents/My Games/Skyrim Special Edition/Logs/Script"
PAPYRUS_LOG="$LOG_DIR/Papyrus.1.log"

rm -f "$PAPYRUS_LOG"

# Launch game via Steam
steam -applaunch 489830 &
PID=$!
sleep 20
kill $PID 2>/dev/null || true

# Wait for log
sleep 2

if [ ! -f "$PAPYRUS_LOG" ]; then
    echo "ERROR: No Papyrus log generated"
    exit 1
fi

echo "=== Papyrus Log ==="
cat "$PAPYRUS_LOG"

echo ""
echo "=== Error Lines ==="
grep -i "error\|cannot open store\|missing file\|unbound" "$PAPYRUS_LOG" || echo "(none)"
