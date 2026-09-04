# Twilight Elysium — vendored engine subset

The files under `third_party/te/` are a minimal, self-contained subset of the
Twilight Elysium engine (`https://github.com/intricos777-dot/twilight-elysium`),
selected so that this project builds with **no external dependencies** (pure
C++20: engine core, memory, input, math types, resolution scaling, entity
system, dummy renderer, shader/pipeline/asset stubs).

- **Copyright (c) 2026 Joshua Allen Cole Scott. All rights reserved.**
- Vendored from `twilight-elysium` at the state matching the `loz-majoras-mask`
  bridge work (2026-08); two additions on top of upstream are marked in source:
  - `te::create_renderer()` factory in `src/renderer/renderer.{h,cpp}`
- No license is granted for these files; they are included for the convenience
  of building the `mm` engine-core demo.

See `/COPYRIGHT.txt` for the repository-wide notice.