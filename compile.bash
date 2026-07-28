#!/usr/bin/env bash

set -e

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

CPP_DIR="$ROOT/src/cpp"
BUILD_DIR="$ROOT/build"
GO_DIR="$ROOT/src/go"
OUTPUT_DIR="$ROOT/bin"

# Default build type
BUILD_TYPE="Release"

if [ $# -ge 1 ]; then
    case "$1" in
        debug|Debug)
            BUILD_TYPE="Debug"
            ;;
        release|Release)
            BUILD_TYPE="Release"
            ;;
        *)
            echo "Unknown build type: $1"
            echo "Usage: $0 [debug|release]"
            exit 1
            ;;
    esac
fi

echo "==> Build type: $BUILD_TYPE"

echo "==> Building C++ library"

#mkdir -p "$BUILD_DIR"

cmake -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE"

cmake --build "$BUILD_DIR" -j"$(nproc)"


echo "==> Building Go application"

mkdir -p "$OUTPUT_DIR"

echo "==> Copying shared library"

cp "$BUILD_DIR/libsearch.so" "$OUTPUT_DIR/"

cd "$GO_DIR"

echo "==> Compiling go executable"

CGO_ENABLED=1 \
go build -ldflags '-linkmode external -extldflags "-Wl,-rpath,$ORIGIN"' \
    -o "$OUTPUT_DIR"/search-server

echo "==> Done"
