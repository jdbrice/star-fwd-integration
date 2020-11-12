#!/usr/bin/env bash
set -euo pipefail

son=${2:-$1}
echo "Fast Installing target: $1 as lib$son.so"
echo cp /tmp/star-build/StRoot/$1/lib$1.so /tmp/star-install/lib/lib${son}.so
cp /tmp/star-build/StRoot/$1/lib$1.so /tmp/star-install/lib/lib${son}.so