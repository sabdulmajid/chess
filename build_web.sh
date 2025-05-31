#!/bin/bash

# Build script for WebAssembly version using Emscripten

set -e

echo "Building Chess Engine for Web..."

# Check if Emscripten is installed
if ! command -v emcc &> /dev/null; then
    echo "Error: Emscripten not found. Please install Emscripten SDK."
    echo "Visit: https://emscripten.org/docs/getting_started/downloads.html"
    exit 1
fi

# Create build directory
mkdir -p build/web

# Compile source files
echo "Compiling source files..."

SOURCES=(
    "src/pieces/piece.cpp"
    "src/pieces/bishop.cpp" 
    "src/pieces/king.cpp"
    "src/pieces/knight.cpp"
    "src/pieces/pawn.cpp"
    "src/pieces/queen.cpp"
    "src/pieces/rook.cpp"
    "src/board/chessboard.cpp"
    "src/game/game.cpp"
    "src/players/player.cpp"
    "src/players/human.cpp"
    "src/players/computer.cpp"
    "src/observers/textobserver.cpp"
    "src/web/web_interface.cpp"
)

INCLUDE_DIRS=(
    "-Iinclude"
    "-Iinclude/pieces"
    "-Iinclude/board"
    "-Iinclude/game"
    "-Iinclude/players"
    "-Iinclude/observers"
    "-Iinclude/web"
)

EMCC_FLAGS=(
    "-std=c++17"
    "-O3"
    "-s WASM=1"
    "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
    "-s ALLOW_MEMORY_GROWTH=1"
    "-s MODULARIZE=1"
    "-s EXPORT_NAME='ChessModule'"
    "-s ENVIRONMENT='web'"
    "-s FILESYSTEM=0"
    "-s MALLOC=emmalloc"
    "-DWEB_VERSION"
    "--bind"
    "--pre-js web/pre.js"
)

# Build command
emcc "${SOURCES[@]}" "${INCLUDE_DIRS[@]}" "${EMCC_FLAGS[@]}" -o build/web/chess.js

echo "Copying web assets..."
cp web/index.html build/web/
cp web/styles.css build/web/
cp web/game.js build/web/

echo "Web build complete! Files are in build/web/"
echo "To serve locally, run: python3 -m http.server 8000 --directory build/web"
