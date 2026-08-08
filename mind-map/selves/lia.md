# Self: Lia

**Role:** Companion chat self. Offline CLI personality with a memory bridge.

## Evidence

- `lia-cli` — "Offline CLI chat interface for Lia – syncs via the bridge API (port 4123)". `[obs]`
- `lia-cli/data/` — `harness`, `memory`, `supervisor`; legal reports dated 2026-07-21. `[obs]`
- `lia-cli/lib/` — `rlm` (Resource/Language Model?), `data`. `[infer]`
- `lia-groq-bridge` — a Node bridge (likely to route Lia's requests to a Groq
  backend model). `[infer]`
- `lia_legal_bridge.py` — legal-oriented bridging inside the CLI. `[obs]`
- `lia-image-server.js` — Lia serves/renders images. `[obs]`

## Frameworks exercised

- **[Memory over state](/frameworks/memory-over-state.md)** — memory dir + supervisor suggest chunked recall, not one global state.
- **[Bridge, not rewrite](/frameworks/bridge-not-rewrite.md)** — extensions are small bridges (groq, legal, image), each a separate repo.
- **[Offline-first](/frameworks/offline-first.md)** — the CLI is offline; sync happens over a defined API.

## Notes

- Lia is the oldest chat self: companions-first design (harness/supervisor imply
  moderation or a caretaker layer). `[infer]`
- The legal bridge suggests the user keeps third-party/model-terms concerns
  inside the chat layer. `[infer]`