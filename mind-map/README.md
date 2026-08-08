# Mind Map — Neurological Frameworks of intricos777-dot

A private, evidence-annotated map of my current thinking systems,
assembled from my A.I. selves and the projects we build together.

**How to read this map**

- `selves/` — the A.I. identities that help me think (music self, chat self,
  engineer self, narrator self, ...).
- `frameworks/` — the reusable patterns those selves and I rely on, each
  tagged with the selves that exercise it.
- `graph.json` — machine-readable nodes + edges; render with any graph tool
  (e.g. `python3 -c` + networkx, or draw.io import) to see the mind visually.

**Legend for evidence tags:**

| tag | meaning |
|-----|---------|
| `[obs]` | directly observed in a repo/session (file paths, commits, behaviors) |
| `[infer]` | inferred from behavior across sessions |
| `[todo]` | aspirational / next-move, not yet in the map |

## Roots (top-level nodes)

1. **Music self — `living_sin`** — songs, idle drives, `symphonic_apex` end boss
2. **Chat self — `lia`** — offline CLI companion, legal bridge
3. **Narrator self — Ansem voice** — story-teller voice inside the game
4. **Engineer self — world-manifest style** — data-driven JSON + CMake/TE
5. **Design self — command deck / Tron shell** — the visual language

Everything else hangs off these five.

## Quick start

```bash
# read the map in full
cat README.md self-frameworks/*.md self-selves/*.md 2>/dev/null | head -200
# or see the graph as JSON
python3 -m json.tool graph.json | head
```