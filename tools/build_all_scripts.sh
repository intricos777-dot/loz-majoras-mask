#!/usr/bin/env bash
set -e
SKYRIM_DIR="/home/sin/.local/share/Steam/steamapps/common/Skyrim Special Edition"
PAPYRUS="$SKYRIM_DIR/Papyrus Compiler/PapyrusCompiler.exe"
COMPAT_DIR="/home/sin/.steam/steam/steamapps/compatdata/489830/pfx"
SKYRIM_SCRIPTS_SRC="$SKYRIM_DIR/Data/Scripts/Source"
PEX_DIR="$SKYRIM_DIR/Data/Scripts"

# Kill stale wineserver
killall -9 wineserver 2>/dev/null || true
sleep 1

echo "========================================="
echo "Building ALL Papyrus Scripts"
echo "========================================="

export WINEPREFIX="$COMPAT_DIR"
export DXVK_ASYNC=1

# 1. Build RealisticAI scripts
echo ""
echo "--- Step 1: Preprocess RealisticAI scripts ---"
cd /home/sin/Projects/skyrim-ai-overhaul
mkdir -p Build/Scripts/Source Build/Scripts
# Run Node.js preprocessor
node Tools/compile-pex.js 2>&1
echo "Preprocessing done"

echo ""
echo "--- Step 2: Compile RealisticAI scripts ---"
PSC_DIR="/home/sin/Projects/skyrim-ai-overhaul/Build/Scripts/Source"
PBUILD_DIR="/home/sin/Projects/skyrim-ai-overhaul/Build/Scripts"

# Write Debug.psc stub for PapyrusCompiler
echo 'Scriptname Debug
Function Trace(string asText) global
EndFunction
Function MessageBox(string asText) global
EndFunction
Function Notification(string asText) global
EndFunction' > "$PSC_DIR/Debug.psc"

wine "$PAPYRUS" \
    -all \
    -i "$PSC_DIR" \
    -h "$SKYRIM_SCRIPTS_SRC" \
    -h "$PSC_DIR" \
    -o "$PBUILD_DIR" \
    -O \
    -nocache 2>&1 | grep -v "fixme:\|libEGL\|pci id\|print_backtrace" || true

echo ""
echo "RealisticAI scripts compiled:"
ls -la "$PBUILD_DIR"/*.pex 2>/dev/null || echo "(no files produced)"

# 2. Build Elsewhyr scripts
echo ""
echo "--- Step 3: Preprocess Elsewhyr scripts ---"
ELSEWHYR_SRC="/home/sin/Projects/The-Elder-Scrolls-Elsewhyr/scripts/source"
ELSEWHYR_BUILD="/home/sin/Projects/The-Elder-Scrolls-Elsewhyr/build_output/scripts"
mkdir -p "$ELSEWHYR_BUILD"

# Compile each Elsewhyr script individually (they don't need preprocessing)
for f in "$ELSEWHYR_SRC"/*.psc; do
    base=$(basename "$f")
    echo "  Compiling $base..."
    wine "$PAPYRUS" \
        "$f" \
        -i "$ELSEWHYR_SRC" \
        -i "$PSC_DIR" \
        -h "$SKYRIM_SCRIPTS_SRC" \
        -o "$ELSEWHYR_BUILD" \
        -O \
        -nocache 2>&1 | grep -v "fixme:\|libEGL\|pci id\|print_backtrace" || echo "  FAILED (but continuing)"
done

echo ""
echo "Elsewhyr scripts compiled:"
ls -la "$ELSEWHYR_BUILD"/*.pex 2>/dev/null || echo "(no files produced)"

# 3. Build Narrative scripts
echo ""
echo "--- Step 4: Compile Narrative scripts ---"
NARR_SRC="$SKYRIM_SCRIPTS_SRC"
NARR_BUILD="/tmp/narrative_pex"
mkdir -p "$NARR_BUILD"
if [ -f "$NARR_SRC/NarrativePapyrus.psc" ]; then
    wine "$PAPYRUS" \
        "$NARR_SRC/NarrativePapyrus.psc" \
        -i "$NARR_SRC" \
        -i "$PSC_DIR" \
        -h "$SKYRIM_SCRIPTS_SRC" \
        -o "$NARR_BUILD" \
        -O \
        -nocache 2>&1 | grep -v "fixme:\|libEGL\|pci id\|print_backtrace" || true
fi
if [ -f "$NARR_SRC/MainQuestline.psc" ]; then
    wine "$PAPYRUS" \
        "$NARR_SRC/MainQuestline.psc" \
        -i "$NARR_SRC" \
        -i "$PSC_DIR" \
        -h "$SKYRIM_SCRIPTS_SRC" \
        -o "$NARR_BUILD" \
        -O \
        -nocache 2>&1 | grep -v "fixme:\|libEGL\|pci id\|print_backtrace" || true
fi

echo ""
echo "Narrative scripts compiled:"
ls -la "$NARR_BUILD"/*.pex 2>/dev/null || echo "(no files produced)"

# 4. Deploy all to Data/Scripts
echo ""
echo "--- Step 5: Deploy to $PEX_DIR ---"
mkdir -p "$PEX_DIR"
cp -v "$PBUILD_DIR"/*.pex "$PEX_DIR"/ 2>/dev/null || true
cp -v "$ELSEWHYR_BUILD"/*.pex "$PEX_DIR"/ 2>/dev/null || true
cp -v "$NARR_BUILD"/*.pex "$PEX_DIR"/ 2>/dev/null || true

echo ""
echo "========================================="
echo "Deployed .pex files:"
ls -la "$PEX_DIR"/*.pex 2>/dev/null || echo "(empty)"
echo "========================================="
