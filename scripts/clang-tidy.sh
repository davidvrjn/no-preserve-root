#!/usr/bin/env sh
# scripts/clang-tidy.sh
# Run clang-tidy over project files, filter only noisy compiler-summary lines,
# preserve clang-tidy exit codes, and print only useful diagnostics.

set -eu

# ensure compile DB exists
rm -f compile_commands.json
if ! command -v bear >/dev/null 2>&1; then
  echo "Please install 'bear' (apt: apt-get install bear)" >&2
  exit 2
fi
bear -- make -B -j2

#!/bin/bash
set -o pipefail
# Execute clang-tidy and filter out lines containing "warnings generated"
jq -r '.[].file' compile_commands.json | sort -u | grep -E '/(src|include)/' | \
  xargs -r -n1 clang-tidy -p . -quiet -checks='clang-analyzer-*,bugprone-*' | \
  grep -v 'warnings generated'

