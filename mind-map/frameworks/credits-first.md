# Framework: Credits-First

**What it is:** Attribution is a first-class data structure, decided at the
moment the asset enters the project — not a compliance chore afterwards.

## Evidence

- `assets/audio/credits.json` maps *every* clip to creator + YouTube URL.
- `AudioPlayer::credit_for()` returns it; the Tron shell prints the track
  name + artist live in the HUD.
- The artist self (`living_sin`) is credited in-game, not just in a THANKS.txt.
- `.gitignore` keeps `assets/audio/` local — the *attribution* stays in-repo.

## Rule of thumb

> A clip without a credit row does not play.

## Who exercises it

living_sin, The Engineer.

## Carry

- Add `credits.json` rows before wiring a new clip into `music.json`.
- Credit the artist in the product surface (HUD/terminal), not just the repo.