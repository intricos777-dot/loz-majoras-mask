# The Legend of Zelda: Majora's Mask — PC Build

A fork-ready home for two ways to play *Majora's Mask* on modern hardware:

- **Track A — `mm` engine core (Twilight Elysium bridge).** A small,
  self-contained C++20 engine-core demo: the 3-day time cycle, Moon fall,
  Song of Time rewind, and player movement math. Builds anywhere CMake works,
  zero external dependencies, headless self-test included.
- **Track B — 2 Ship 2 Harkinian (recommended).** The full community
  decompilation port of *Majora's Mask* by HarbourMasters, pinned to release
  **5.0.1** and wired up with one-command build scripts for Linux, macOS and
  Windows. This is the "gigachad" path — a modern, widescreen, high-res,
  moddable PC port.

> **You must own an original copy of The Legend of Zelda: Majora's Mask.**
> This project contains **no Nintendo assets** — the game extracts them from
> *your own* legally-dumped ROM at first launch (`.z64`, `.n64` or `.v64`).

---

## Status

| Track | State |
|---|---|
| A — `mm` engine core | ✅ Builds & self-tests green (13/13 core checks, ctest 1/1) |
| B — 2 Ship 2 Harkinian | ✅ Wiring complete; build one command away (needs your ROM) |

---

## Track A — build the `mm` engine core (fast)

The engine core is dependency-free C++20. It prints a self-test of the
3-day clock and movement physics (`mm --core`).

### Linux / macOS
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/mm --core
```
or simply:
```bash
./scripts/build-mm.sh
```

### Windows (PowerShell)
```powershell
.\scripts\build-mm.ps1
```
(uses Ninja if present, otherwise the Visual Studio 2022 generator; requires
CMake + a C++20 toolchain).

---

## Track B — build the full game via 2 Ship 2 Harkinian (recommended)

One command installs dependencies, clones upstream **2S2H 5.0.1**, and builds
the port. If you place a Majora's Mask ROM next to the script, it is
auto-extracted; otherwise the game asks you to locate your ROM on first launch
(built-in file picker).

### Linux / macOS
```bash
./scripts/setup-2ship.sh
```
Run the result, then point it at your ROM when asked:
```bash
./2ship2harkinian/build-cmake/mm/2s2h.elf      # Linux
./2ship2harkinian/build-cmake/mm/2s2h-macos    # macOS
```
Supported distros for dependency install: Arch, Debian/Ubuntu, Fedora,
openSUSE, and macOS via Homebrew.

### Windows (PowerShell)
```powershell
.\scripts\setup-2ship.ps1
```
Requires Visual Studio 2022 ("Desktop development with C++", v143), Git and
CMake. Run the produced `2s2h.exe`:
```powershell
.\2ship2harkinian\build\x64\Release\2s2h.exe
```

### If you want to extract assets from a ROM during setup
```bash
# Linux/macOS — drop the ROM in the repo root before running the script
./scripts/setup-2ship.sh

# Windows — pass the path explicitly
.\scripts\setup-2ship.ps1 -RomPath "C:\Games\MajorasMask.z64"
```

### Notes
- See upstream `docs/BUILDING.md` (mirrored in your local `2ship2harkinian`
  clone) for advanced targets: `Generate2ShipOtr`, packaging with `cpack`,
  and the `ExtractAssetHeaders` workflow.
- 2S2H release binaries and nightly builds are also published by HarbourMasters
  — the scripts here exist so you always build from source, reproducibly.

---

## Project layout

```
src/                  # mm engine-core bridge: time cycle, player movement
tests/                # python smoke tests (expect ./build/mm)
third_party/te/       # vendored Twilight Elysium engine subset (see NOTICE.md)
scripts/              # build-mm / setup-2ship for Linux, macOS and Windows
CMakeLists.txt        # builds the self-contained mm core
```

## Versions & hashes

- 2S2H pinned tag: `5.0.1` (upstream `HarbourMasters/2ship2harkinian`)
- Supported ROM hashes (from upstream `docs/supportedHashes.json`) — the game
  verifies your dump against these before extracting.

## Credits & licensing

- **2 Ship 2 Harkinian** is a community project by HarbourMasters, released
  under **CC0-1.0**. The scripts in this repo only orchestrate an unmodified
  upstream build.
- **`mm` engine core** and the vendored **Twilight Elysium** subset:
  Copyright (c) 2026 Joshua Allen Cole Scott. All rights reserved — see
  `COPYRIGHT.txt` and `third_party/te/NOTICE.md`.
- *The Legend of Zelda: Majora's Mask* and its assets are the property of
  Nintendo. This project is not affiliated with or endorsed by Nintendo.