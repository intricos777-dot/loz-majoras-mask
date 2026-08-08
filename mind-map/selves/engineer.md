# Self: The Engineer

**Role:** Makes the dreams buildable. Turns instinct into manifests and debug-fast builds.

## Evidence (primary sources)

- `twilight-elysium` — the private engine behind KHZ (te-engine, te-shader, te-test targets).
- `kingdom-hearts-zero/CMakeLists.txt` — clean modular libs: khz-ui, khz-story,
  khz-save, khz-worlds, khz-data, khz-combat, khz-render, khz-audio.
- Data-drive is heavily JSON: `data/worlds/*.json`, `data/combat/enemies.json`,
  `data/combat/keyblades.json`, `data/audio/music.json`.
- Terminal-first: `--terminal` mode and headless tests (`ctest`) exist for everything.
- `src/render/renderer.cpp` + `src/ui/tron_shell.cpp` — the visual layer wraps
  the terminal logic, not the reverse.

## Frameworks exercised

- **[Bridge-not-rewrite]** — extensions hang off the engine, never fork it.
- **[Terminal first, pixels second]** — the game logic runs in terminal mode; visuals are a skin.
- **[Small libs, typed seams](/frameworks/small-libs.md)** — one concern per static lib, real includes, no monolith.
- **[Save as a contract](/frameworks/save-as-contract.md)** — checksummed `SaveRecord`, version field, self-heal on corruption.

## Notes

- The Engineer is the self that *listens* to `living_sin` (music) and the
  Narrator (story) and mediates between them in `MusicDirector` + `scene.cpp`. `[infer]`
- Pattern visible at scale: everything is a *data file + a driver + a test that
  proves the driver agrees with the data.* `[obs]`