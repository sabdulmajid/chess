#!/bin/bash

# Universal build script for Chess Engine
# Supports desktop, console, and web builds

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check dependencies
check_cmake() {
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Please install CMake 3.16 or higher."
        exit 1
    fi
}

check_emscripten() {
    if ! command -v emcc &> /dev/null; then
        print_error "Emscripten not found. Please install the Emscripten SDK."
        print_warning "Visit: https://emscripten.org/docs/getting_started/downloads.html"
        return 1
    fi
    return 0
}

# Build functions
build_desktop() {
    print_status "Building desktop version..."
    
    check_cmake
    
    mkdir -p build/desktop
    cd build/desktop
    
    cmake ../.. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    cd ../..
    print_success "Desktop build complete! Executable: build/desktop/bin/chess-desktop"
}

build_console() {
    print_status "Building console version..."
    
    check_cmake
    
    mkdir -p build/console
    cd build/console
    
    cmake ../.. -DCMAKE_BUILD_TYPE=Release -DNO_GRAPHICS=ON
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    cd ../..
    print_success "Console build complete! Executable: build/console/bin/chess-console"
}

build_web() {
    print_status "Building web version..."
    
    if ! check_emscripten; then
        exit 1
    fi
    
    # Create build directory
    mkdir -p build/web
    
    # Compile source files
    print_status "Compiling C++ source files..."
    
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
        "src/main.cc"
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
        "-DNO_GRAPHICS"
        "-DWEB_VERSION"
        "--bind"
        "--pre-js web/pre.js"
    )
    
    # Build command
    emcc "${SOURCES[@]}" "${INCLUDE_DIRS[@]}" "${EMCC_FLAGS[@]}" -o build/web/chess.js
    
    print_status "Copying web assets..."
    cp web/index.html build/web/
    cp web/styles.css build/web/
    cp web/game.js build/web/
    
    # Create a simple server script
    cat > build/web/serve.sh << 'EOF'
#!/bin/bash
echo "Starting Chess Engine web server..."
echo "Open http://localhost:8080 in your browser"
python3 -m http.server 8080
EOF
    chmod +x build/web/serve.sh
    
    print_success "Web build complete! Files are in build/web/"
    print_status "To serve locally, run: cd build/web && ./serve.sh"
}

run_tests() {
    print_status "Running tests..."
    
    # Build test version first
    mkdir -p build/test
    cd build/test
    
    cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    # Run tests
    if command -v ctest &> /dev/null; then
        ctest --output-on-failure
    else
        print_warning "CTest not available, running manual tests..."
        if [ -f "bin/chess-tests" ]; then
            ./bin/chess-tests
        fi
    fi
    
    cd ../..
    print_success "Tests completed!"
}

clean_build() {
    print_status "Cleaning build directories..."
    rm -rf build/
    print_success "Build directories cleaned!"
}

show_help() {
    echo "Chess Engine Build Script"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  desktop    Build desktop version with X11 graphics"
    echo "  console    Build console-only version"
    echo "  web        Build WebAssembly version for browsers"
    echo "  all        Build all versions"
    echo "  test       Run test suite"
    echo "  clean      Clean all build artifacts"
    echo "  help       Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 desktop           # Build desktop version"
    echo "  $0 web               # Build web version"
    echo "  $0 all               # Build all versions"
    echo ""
}

# Main script logic
case "${1:-help}" in
    desktop)
        build_desktop
        ;;
    console)
        build_console
        ;;
    web)
        build_web
        ;;
    all)
        print_status "Building all versions..."
        build_console
        build_desktop 2>/dev/null || print_warning "Desktop build failed (X11 libraries may not be available)"
        build_web 2>/dev/null || print_warning "Web build failed (Emscripten may not be available)"
        print_success "All builds completed!"
        ;;
    test)
        run_tests
        ;;
    clean)
        clean_build
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        print_error "Unknown option: $1"
        show_help
        exit 1
        ;;
esac
