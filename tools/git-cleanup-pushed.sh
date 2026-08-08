#!/usr/bin/env bash
# git-cleanup-pushed.sh
# Deletes local git repos that are clean and fully pushed to origin.
# Keeps repos with uncommitted changes, unpushed commits, or no remote.
# Asks for confirmation before deleting.

set -euo pipefail

ROOT="${1:-/home/sin/Projects}"
KEEP_LIST=(
  "ringworld-redux"
  "twilight-elysium"
  "mp3-video-virtualizer"
  "living-sin-assets"
)

is_kept() {
  local name="$1"
  for k in "${KEEP_LIST[@]}"; do
    if [[ "$name" == "$k" ]]; then
      return 0
    fi
  done
  return 1
}

declare -a TO_DELETE=()
declare -a KEPT_REASONS=()

while IFS= read -r gitdir; do
  repo_dir="$(dirname "$gitdir")"
  repo_name="$(basename "$repo_dir")"

  cd "$repo_dir" || continue

  # Skip keep list
  if is_kept "$repo_name"; then
    KEPT_REASONS+=("KEEP(list): $repo_name")
    continue
  fi

  # Skip if no remote
  remote="$(git remote get-url origin 2>/dev/null || true)"
  if [[ -z "$remote" ]]; then
    KEPT_REASONS+=("KEEP(no remote): $repo_name")
    continue
  fi

  # Skip if dirty
  if [[ -n "$(git status --porcelain 2>/dev/null)" ]]; then
    KEPT_REASONS+=("KEEP(dirty): $repo_name")
    continue
  fi

  # Skip if unpushed commits
  ahead="$(( $(git rev-list --count @{u}..HEAD 2>/dev/null || echo 0) ))"
  if [[ "$ahead" -gt 0 ]]; then
    KEPT_REASONS+=("KEEP(unpushed): $repo_name ($ahead commits ahead)")
    continue
  fi

  # Repo is clean and pushed
  TO_DELETE+=("$repo_dir|$repo_name")
done < <(find "$ROOT" -maxdepth 2 -type d -name '.git' 2>/dev/null)

echo "=== KEPT REPOS ==="
for r in "${KEPT_REASONS[@]}"; do
  echo "  $r"
done

echo ""
echo "=== CANDIDATES FOR DELETION (clean + pushed) ==="
if [[ ${#TO_DELETE[@]} -eq 0 ]]; then
  echo "  None."
  exit 0
fi

for entry in "${TO_DELETE[@]}"; do
  IFS='|' read -r dir name <<< "$entry"
  echo "  $name -> $dir"
done

echo ""
read -rp "Delete these ${#TO_DELETE[@]} local repos? [y/N] " confirm
if [[ "$confirm" != [yY] ]]; then
  echo "Aborted."
  exit 0
fi

for entry in "${TO_DELETE[@]}"; do
  IFS='|' read -r dir name <<< "$entry"
  rm -rf "$dir"
  echo "DELETED: $name"
done

echo ""
echo "Done. Remaining repos under $ROOT:"
find "$ROOT" -maxdepth 2 -type d -name '.git' -printf '%h\n' 2>/dev/null | sed "s|$ROOT/||" | sort
