#!/usr/bin/env bash

###############################################################################
# 📦 build.sh – Build-Skript für cd_iso_to_wav
# Autor      : Matthias Stoltze
# Jahr       : 2025
# Lizenz     : MIT (oder eigene Lizenz hier eintragen)
###############################################################################

# ⛔ Kein Parameter? Dann Hilfe anzeigen
if [[ -z "$1" || -z "$2" ]]; then
  echo ""
  echo "╔═══════════════════════════════════════════════════════════════╗"
  echo "║             🧾 cd_iso_to_wav Build-Skript                     ║"
  echo "║             © 2025 Matthias Stoltze                           ║"
  echo "╠═══════════════════════════════════════════════════════════════╣"
  echo "║ Verwendung:                                                   ║"
  echo "║   ./build.sh <toolchain> <build-type>                         ║"
  echo "║                                                               ║"
  echo "║ Parameter:                                                    ║"
  echo "║   <toolchain>   ucrt64 | mingw64                              ║"
  echo "║   <build-type>  debug   | release                             ║"
  echo "║                                                               ║"
  echo "║ Beispiel:                                                     ║"
  echo "║   ./build.sh mingw64 release                                  ║"
  echo "╚═══════════════════════════════════════════════════════════════╝"
  echo ""
  exit 0
fi

set -e

TOOLCHAIN="$1"
BUILD_TYPE="$2"

# 🔎 Toolchain prüfen
if [[ "$TOOLCHAIN" != "ucrt64" && "$TOOLCHAIN" != "mingw64" ]]; then
  echo "❌ Ungültiger Toolchain-Parameter: '$TOOLCHAIN'"
  echo "➡️  Gültige Optionen: ucrt64 oder mingw64"
  exit 1
fi

# 🔎 Build-Typ prüfen
if [[ "$BUILD_TYPE" != "debug" && "$BUILD_TYPE" != "release" ]]; then
  echo "❌ Ungültiger Build-Typ: '$BUILD_TYPE'"
  echo "➡️  Gültige Optionen: debug oder release"
  exit 1
fi

# 🛠️ Toolchain-Binaries holen
TOOLCHAIN_BIN_WIN="C:/msys64/$TOOLCHAIN/bin"
GCC="$(cygpath -u "$TOOLCHAIN_BIN_WIN/gcc.exe")"
GXX="$(cygpath -u "$TOOLCHAIN_BIN_WIN/g++.exe")"
MAKE="$(cygpath -u "$TOOLCHAIN_BIN_WIN/mingw32-make.exe")"

# 📁 Build-Ordner vorbereiten
PROJECT_DIR="$(pwd)"
BUILD_TYPE_UPPER="$(tr '[:lower:]' '[:upper:]' <<< "$BUILD_TYPE")"
BUILD_DIR="$PROJECT_DIR/build-$TOOLCHAIN-$BUILD_TYPE"
INSTALL_DIR="$PROJECT_DIR/dist/$TOOLCHAIN/$BUILD_TYPE"

echo "🔧 [1/4] Konfiguriere $TOOLCHAIN ($BUILD_TYPE_UPPER)..."
cmake -B "$BUILD_DIR" -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE_UPPER" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_C_COMPILER="$GCC" \
  -DCMAKE_CXX_COMPILER="$GXX" \
  -DCMAKE_MAKE_PROGRAM="$MAKE"

echo "🛠️  [2/4] Baue das Projekt..."
cmake --build "$BUILD_DIR" --parallel

echo "📦 [3/4] Installiere nach $INSTALL_DIR..."
rm -rf "$INSTALL_DIR"
cmake --install "$BUILD_DIR" --prefix "$INSTALL_DIR"

echo "✅ [4/4] Fertig! Executable liegt in: $INSTALL_DIR/"
