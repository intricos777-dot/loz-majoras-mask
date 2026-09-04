#!/usr/bin/env bash
# Full 2 Ship 2 Harkinian (2S2H) build for Linux/macOS - Track B (recommended).
#
# What this does:
#   1. Installs build dependencies for your distro (Arch/Debian/Fedora/openSUSE/macOS)
#   2. Clones upstream 2S2H pinned to release 5.0.1 (CC0-licensed decomp port)
#   3. Configures and builds the game (Ninja)
#   4. Optional: if you place a legally-dumped Majora's Mask ROM (.z64/.n64/.v64)
#      in this folder, it passes it to the asset extractor automatically.
#      If you don't, the game asks you to pick your ROM on first launch.
#
# You must own an original copy of The Legend of Zelda: Majora's Mask.
# No Nintendo assets are distributed by this project.
set -euo pipefail

cd "$(dirname "$0")/.."
ROOT="$(pwd)"
TAG="${2S2H_TAG:-5.0.1}"
DIR="${2S2H_DIR:-$ROOT/2ship2harkinian}"
JOBS="${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

# --- 1. dependencies ---------------------------------------------------------
install_deps() {
  if command -v pacman >/dev/null 2>&1; then
    sudo pacman -S --needed --noconfirm gcc git cmake ninja lsb-release sdl2 libpng \
      libzip nlohmann-json tinyxml2 spdlog sdl2_net libogg libvorbis opus opusfile
  elif command -v apt-get >/dev/null 2>&1; then
    sudo apt-get update
    sudo apt-get install -y gcc g++ git cmake ninja-build lsb-release libsdl2-dev \
      libpng-dev libsdl2-net-dev libzip-dev zipcmp zipmerge ziptool nlohmann-json3-dev \
      libtinyxml2-dev libspdlog-dev libopengl-dev libopus-dev libopusfile-dev \
      libogg-dev libvorbis-dev
  elif command -v dnf >/dev/null 2>&1; then
    sudo dnf install -y gcc gcc-c++ git cmake ninja-build lsb_release SDL2-devel \
      libpng-devel libzip-devel libzip-tools nlohmann-json-devel tinyxml2-devel \
      spdlog-devel libogg-devel libvorbis-devel opus-devel opusfile-devel
  elif command -v zypper >/dev/null 2>&1; then
    sudo zypper install -y gcc gcc-c++ git cmake ninja SDL2-devel libpng16-devel \
      libzip-devel libzip-tools nlohmann_json-devel tinyxml2-devel spdlog-devel \
      libogg-devel libvorbis-devel opus-devel opusfile-devel
  elif command -v brew >/dev/null 2>&1; then
    brew install sdl2 libpng glew ninja cmake nlohmann-json libzip
  else
    echo "Unsupported package manager - install the deps listed in docs/BUILDING.md of 2S2H." >&2
  fi
}

# --- 2. clone ----------------------------------------------------------------
ensure_source() {
  if [ ! -d "$DIR/.git" ]; then
    echo "==> Cloning 2Ship2Harkinian @ $TAG"
    git clone --depth 1 --branch "$TAG" https://github.com/HarbourMasters/2ship2harkinian.git "$DIR"
  fi
  ( cd "$DIR" && git submodule update --init --depth 1 )
}

# --- 3. build ----------------------------------------------------------------
build() {
  local ROM=""
  # Find a user-supplied ROM next to this script (also accept it in 2ship dir).
  for pat in "$ROOT"/*.z64 "$ROOT"/*.n64 "$ROOT"/*.v64 "$ROOT"/mm/*.z64; do
    [ -f "$pat" ] && ROM="${ROM:-$pat}"
  done

  cmake -S "$DIR" -B "$DIR/build-cmake" -GNinja -DCMAKE_BUILD_TYPE=Release

  if [ -n "$ROM" ]; then
    echo "==> ROM found: $ROM - extracting assets (mm.o2r)"
    mkdir -p "$DIR/mm"
    cp "$ROM" "$DIR/mm/mm.selected.n64"
    cmake --build "$DIR/build-cmake" --target Generate2ShipOtr
  fi

  echo "==> Compiling (jobs=$JOBS)"
  cmake --build "$DIR/build-cmake" -j "$JOBS"
}

# --- 4. run ------------------------------------------------------------------
run() {
  local BIN
  if [ -f "$DIR/build-cmake/mm/2s2h.elf" ]; then BIN="$DIR/build-cmake/mm/2s2h.elf";
  elif [ -f "$DIR/build-cmake/mm/2s2h-macos" ]; then BIN="$DIR/build-cmake/mm/2s2h-macos"; fi

  echo
  echo "Build complete. Run with:"
  echo "  $BIN"
  echo "(If no ROM was extracted, the game will ask you to locate your own Majora's Mask .z64/.n64/.v64 on first launch.)"
}

install_deps
ensure_source
build
run