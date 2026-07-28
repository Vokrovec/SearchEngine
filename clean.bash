#!/usr/bin/env bash

set -e

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

#CPP_DIR="$ROOT/src/cpp"
BUILD_DIR="$ROOT/build"
#GO_DIR="$ROOT/src/go"
OUTPUT_DIR="$ROOT/bin"
URLS_DIR="$ROOT/urls"

cd ./src/go
  go clean -cache
cd -
rm -rf "$URLS_DIR" "$BUILD_DIR" "$OUTPUT_DIR"
