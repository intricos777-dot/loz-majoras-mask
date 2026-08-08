#!/usr/bin/env bash
# Launch Skyrim SE with SKSE64 - single instance, proper cleanup
set -e

LOCKFILE="/tmp/skyrim.lock"
SKYRIM_DIR="/home/sin/.local/share/Steam/steamapps/common/Skyrim Special Edition"
COMPAT_DATA="/home/sin/.steam/steam/steamapps/compatdata/489830"
COMPAT_CLIENT="/home/sin/.local/share/Steam"
PAPYRUS_LOG="$COMPAT_DATA/pfx/drive_c/users/steamuser/Documents/My Games/Skyrim Special Edition/Logs/Script/Papyrus.0.log"

cleanup() {
    echo "=== Cleaning up old instances ==="
    # Kill ALL related processes
    for p in SkyrimSE.exe skse64_loader.exe; do
        pkill -9 -f "$p" 2>/dev/null || true
    done
    # Kill wine processes
    wineserver -k 2>/dev/null || true
    pkill -9 -f winedevice.exe 2>/dev/null || true
    pkill -9 -f xalia.exe 2>/dev/null || true
    sleep 2
    # Verify
    if pgrep -f "SkyrimSE.exe" >/dev/null 2>&1; then
        echo "WARNING: Some Skyrim processes could not be killed"
        pgrep -f "SkyrimSE.exe" | while read pid; do
            kill -9 "$pid" 2>/dev/null || true
        done
        sleep 1
    fi
    rm -f "$LOCKFILE"
    echo "Cleanup done"
}

# Check lock
if [ -f "$LOCKFILE" ]; then
    OLD_PID=$(cat "$LOCKFILE")
    if kill -0 "$OLD_PID" 2>/dev/null; then
        echo "ERROR: Skyrim is already running (PID $OLD_PID)"
        echo "Use -f to force launch"
        exit 1
    else
        echo "Stale lock found (PID $OLD_PID), removing"
        rm -f "$LOCKFILE"
    fi
fi

# Force flag
if [ "$1" = "-f" ]; then
    cleanup
fi

# Normal cleanup before launch
cleanup

# Create lock
echo "$$" > "$LOCKFILE"

# Clear logs
rm -f "$SKYRIM_DIR/skse64.log" "$PAPYRUS_LOG"

# Launch
cd "$SKYRIM_DIR"
export STEAM_COMPAT_DATA_PATH="$COMPAT_DATA"
export STEAM_COMPAT_CLIENT_INSTALL_PATH="$COMPAT_CLIENT"
export DXVK_ASYNC=1
export PROTON_ENABLE_NVAPI=1

SKYRIM_DIR_WIN="Z:\\$(echo "$SKYRIM_DIR" | sed 's|/|\\\\|g')"
echo "Launching SKSE64 via Proton..."

# Run SKSE in background, capture PID
"/home/sin/.local/share/Steam/steamapps/common/Proton - Experimental/proton" run skse64_loader.exe &
GAME_PID=$!

# Wait for game process to appear
echo -n "Waiting for game process"
for i in $(seq 1 30); do
    if pgrep -f "SkyrimSE.exe" >/dev/null 2>&1; then
        echo ""
        break
    fi
    echo -n "."
    sleep 1
done

# Monitor
SKYRIM_PID=$(pgrep -f "SkyrimSE.exe" 2>/dev/null || echo "")
if [ -n "$SKYRIM_PID" ]; then
    echo "SkyrimSE.exe running (PID $SKYRIM_PID)"
    echo "PID $SKYRIM_PID" >> "$LOCKFILE"
    echo "Monitor logs at:"
    echo "  $PAPYRUS_LOG"
    echo "  $SKYRIM_DIR/skse64.log"
    echo ""
    echo "To kill: pkill -9 -f SkyrimSE.exe"
else
    echo ""
    echo "WARNING: Game process not detected within 30s"
    echo "Check log: $PAPYRUS_LOG"
fi
