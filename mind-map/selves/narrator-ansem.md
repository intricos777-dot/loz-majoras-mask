# Self: The Narrator (Ansem's voice)

**Role:** Story voice inside Kingdom Hearts 0 — an artificial voice emulation.

## Evidence

- `kingdom-hearts-zero/src/story/scene.cpp` — Ansem narration lines, `voice/` support, styled text. `[obs]`
- Intro video path (`assets/intro/intro.mp4`) with "told in Ansem's voice" fallback. `[obs]`
- Story beats are data-driven per world (`kh1.json`/`kh2.json` `beats`). `[obs]`
- `src/main.cpp` act-one: Ansem narrates map instructions ("Find the keyhole
  in the district. Do not let the dark take your memory."). `[obs]`

## Frameworks exercised

- **[Story-as-data](/frameworks/story-as-data.md)** — scenes and beats are JSON, so AI can generate and extend them.
- **[Voice as a self](/frameworks/voice-as-a-self.md)** — the narrator is a distinct persona with a fixed voice, never the player's.
- **[The Dark as the game](/frameworks/the-dark-as-game.md)** — memory-steal, memory economy, dark-narrator motif across the whole title.

## Notes

- The narrator does the *telling*; the command deck does the *asking*. The two
  selves never overlap. `[infer]`