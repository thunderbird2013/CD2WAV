#!/usr/bin/env bash

set -e


# 🧾 Parameter prüfen
TOOLCHAIN="$1"
if [[ "$TOOLCHAIN" != "ucrt64" && "$TOOLCHAIN" != "mingw64" ]]; then
  echo "❌ Ungültiger Parameter: '$TOOLCHAIN'"
  echo "➡️  Verwende: ./build.sh ucrt64   oder   ./build.sh mingw64"
  exit 1
fi

# 🔧 Toolchain-Wurzel (Windows) und Toolchain-Bin
TOOLCHAIN_BIN_WIN="C:/msys64/$TOOLCHAIN/bin"

# 🔁 Nur Compiler und Make aus Toolchain-Bin, cmake bleibt global
GCC="$(cygpath -u "$TOOLCHAIN_BIN_WIN/gcc.exe")"
GXX="$(cygpath -u "$TOOLCHAIN_BIN_WIN/g++.exe")"
MAKE="$(cygpath -u "$TOOLCHAIN_BIN_WIN/mingw32-make.exe")"

# 📁 Verzeichnisse
PROJECT_DIR="$(pwd)"
BUILD_DIR="$PROJECT_DIR/build-$TOOLCHAIN"
INSTALL_DIR="$PROJECT_DIR/dist"

echo "🔧 [1/4] Konfiguriere mit $TOOLCHAIN..."
cmake -B "$BUILD_DIR" -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_C_COMPILER="$GCC" \
  -DCMAKE_CXX_COMPILER="$GXX" \
  -DCMAKE_MAKE_PROGRAM="$MAKE"

echo "🛠️  [2/4] Baue das Projekt..."
cmake --build "$BUILD_DIR" --parallel

rm -rf "$INSTALL_DIR"

echo "📦 [3/4] Installiere nach $INSTALL_DIR..."
cmake --install "$BUILD_DIR" --prefix "$INSTALL_DIR"

echo "✅ [4/4] Fertig! Executable liegt in: $INSTALL_DIR/"

