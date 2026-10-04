#!/usr/bin/env bash

set -euo pipefail

if [[ $EUID -ne 0 ]]; then
  exec sudo "$0" "$@"
fi

root="$(dirname "$(readlink -f "$0")")"

"$root/build.sh" release

latest_exec=$(
  find "$root/bin" -maxdepth 1 -type f -perm -111 -print0 |
  while IFS= read -r -d '' f; do
    if file -b "$f" | grep -qE 'Mach-O .* executable|ELF .* executable'; then
      stat_out=$(stat -f '%m %N' "$f" 2>/dev/null || stat -c '%Y %n' "$f")
      printf '%s\n' "$stat_out"
    fi
  done |
  sort -nr |
  head -n1 |
  cut -d' ' -f2-
)

if [[ -z ${latest_exec:-} ]]; then
  echo "No binary found in $root/bin" >&2
  exit 1
fi

echo "Installing $(basename "$latest_exec")..."
install -m 755 "$latest_exec" /usr/local/bin/ani
echo Done!
