# Framework: Security-First Automation

**What it is:** Keep private/secret assets out of the public or even the
semi-public repo, and never let automation (especially A.I.-generated code)
introduce secrets or leak risk into commits.

## Evidence

- Saved API tokens are stored in local keyring/chan (e.g. `gh` uses keyring,
  tokens never appear in repo).
- `data/` and `assets/` stay local; binary `*.mp3`/`*.wav`/`*.ogg` ignored
  via `.gitignore` (47 media files, near-commonly not in git status).
- `keygen`, `keys/` dirs in some projects — never committed.
- `SaveSystem` checksums and self-heal guards persisted state against
  tampering/corruption.

## Rule of thumb

> If it's a secret, it's not code. If it's media, it's not git — the shortest
> path to keep the repo lean is `.gitignore`, not `.git/info/exclude`.

## Who exercises it

- The Engineer (saves, checksums)
- GitHub account itself: every repo here is private or archived.

## Carry

- Review diffs for tokens before every commit (git status + git diff).
- Keep keys in keyring/env, not JSON files, when possible.