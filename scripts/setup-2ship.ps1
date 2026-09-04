# Full 2 Ship 2 Harkinian (2S2H) build for Windows - Track B (recommended).
#
# What this does:
#   1. Checks that Visual Studio 2022 (C++ / v143) + CMake + Git are available
#   2. Clones upstream 2S2H pinned to release 5.0.1 (CC0-licensed decomp port)
#   3. Configures and builds the game with the Visual Studio 2022 generator
#   4. Optional: pass -RomPath "C:\path\to\your\game.z64" to auto-extract assets.
#      If you don't, the game asks you to pick your ROM on first launch.
#
# You must own an original copy of The Legend of Zelda: Majora's Mask.
# No Nintendo assets are distributed by this project.
param(
    [string]$Tag = "5.0.1",
    [string]$RomPath = ""
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $PSScriptRoot
$Dir = Join-Path $Root "2ship2harkinian"

function FailIfMissing($Name, $Cmd) {
    if (-not (Get-Command $Cmd -ErrorAction SilentlyContinue)) {
        Write-Error "$Name is required. Install it and re-run. $Cmd"
    }
}

# --- 1. toolchain checks ------------------------------------------------------
FailIfMissing "Git" "git"
FailIfMissing "CMake" "cmake"
$VsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $VsWhere)) {
    Write-Error "Visual Studio 2022 not found. Install 'Desktop development with C++' from https://visualstudio.microsoft.com/ and re-run."
}

# --- 2. clone -----------------------------------------------------------------
if (-not (Test-Path $Dir)) {
    Write-Host "==> Cloning 2Ship2Harkinian @ $Tag"
    git clone --depth 1 --branch $Tag https://github.com/HarbourMasters/2ship2harkinian.git $Dir
}
Push-Location $Dir
git submodule update --init --depth 1

# --- 3. configure + optional ROM extraction -----------------------------------
cmake -S . -B build/x64 -G "Visual Studio 17 2022" -T v143 -A x64 -DCMAKE_BUILD_TYPE=Release

if ($RomPath) {
    if (-not (Test-Path $RomPath)) { Write-Error "ROM not found at $RomPath" }
    Write-Host "==> ROM found: $RomPath - extracting assets (mm.o2r)"
    New-Item -ItemType Directory -Force -Path mm | Out-Null
    Copy-Item $RomPath (Join-Path $PSScriptRoot "2ship2harkinian\mm\mm.selected.n64")
    cmake --build build/x64 --target Generate2ShipOtr --config Release
}

# --- 4. build ------------------------------------------------------------------
Write-Host "==> Compiling"
cmake --build build/x64 --config Release

Pop-Location

# --- 5. report -----------------------------------------------------------------
$Exe = Get-ChildItem -Recurse -Filter "2s2h.exe" $Dir\build | Select-Object -First 1
Write-Host ""
Write-Host "Build complete. Run with:"
Write-Host "  $($Exe.FullName)"
Write-Host "(If no ROM was extracted, the game will ask you to locate your own Majora's Mask .z64/.n64/.v64 on first launch.)"
Write-Host "Optional dist zip: cmake --build $Dir\build/x64 --target package --config Release"