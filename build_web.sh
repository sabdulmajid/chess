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
    "src/ai/advanced_ai.cpp"
    "src/ai/ai_factory.cpp"
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
    "-Iinclude/ai"
)

EMCC_FLAGS=(
    "-std=c++17"
    "-O3"
    "-sWASM=1"
    "-sEXPORTED_RUNTIME_METHODS=ccall,cwrap"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sMODULARIZE=1"
    "-sEXPORT_NAME=ChessModule"
    "-sENVIRONMENT=web"
    "-sFILESYSTEM=0"
    "-sMALLOC=emmalloc"
    "-DWEB_VERSION"
    "-DNO_GRAPHICS"
    "--bind"
    "--pre-js=web/pre.js"
)

# Build command
emcc "${SOURCES[@]}" "${INCLUDE_DIRS[@]}" "${EMCC_FLAGS[@]}" -o build/web/chess.js

echo "Copying web assets..."
cp web/index.html build/web/
cp web/styles.css build/web/
cp web/game.js build/web/

echo "Web build complete! Files are in build/web/"
echo "To serve locally, run: python3 -m http.server 8000 --directory build/web"
