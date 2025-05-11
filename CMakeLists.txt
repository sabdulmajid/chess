cmake_minimum_required(VERSION 3.16)
project(ChessEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(NO_GRAPHICS "Build without graphics support" OFF)
option(ENABLE_TESTING "Enable testing" OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(include)
include_directories(include/pieces)
include_directories(include/board)
include_directories(include/game)
include_directories(include/players)
include_directories(include/observers)
include_directories(include/web)
include_directories(include/graphics)
include_directories(include/ai)

# Find packages and set include paths
find_package(X11)
if(X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
endif()

# Collect source files
file(GLOB_RECURSE PIECE_SOURCES "src/pieces/*.cpp")
file(GLOB_RECURSE BOARD_SOURCES "src/board/*.cpp")
file(GLOB_RECURSE GAME_SOURCES "src/game/*.cpp")
file(GLOB_RECURSE PLAYER_SOURCES "src/players/*.cpp")
file(GLOB_RECURSE AI_SOURCES "src/ai/*.cpp")

# Observer sources (conditionally exclude graphics observer)
if(NO_GRAPHICS)
    file(GLOB OBSERVER_SOURCES "src/observers/textobserver.cpp")
else()
    file(GLOB_RECURSE OBSERVER_SOURCES "src/observers/*.cpp")
endif()

# Graphics sources (only if enabled)
if(NOT NO_GRAPHICS)
    file(GLOB_RECURSE GRAPHICS_SOURCES "src/graphics/*.cpp" "src/graphics/*.cc")
endif()

# Create core library
add_library(ChessCore STATIC
    ${PIECE_SOURCES}
    ${BOARD_SOURCES}
    ${GAME_SOURCES}
    ${PLAYER_SOURCES}
    ${AI_SOURCES}
    ${OBSERVER_SOURCES}
)

# Conditional compilation definitions
if(NO_GRAPHICS)
    target_compile_definitions(ChessCore PUBLIC NO_GRAPHICS)
endif()

# Desktop application with X11 graphics
if(NOT NO_GRAPHICS AND X11_FOUND)
    add_library(ChessGraphics STATIC ${GRAPHICS_SOURCES})
    target_link_libraries(ChessGraphics ${X11_LIBRARIES})
    
    add_executable(chess-desktop src/main.cc)
    target_link_libraries(chess-desktop ChessCore ChessGraphics ${X11_LIBRARIES})
endif()

# Console-only version (only build if NO_GRAPHICS is set)
if(NO_GRAPHICS)
    add_executable(chess-console src/main.cc)
    target_link_libraries(chess-console ChessCore)
    target_compile_definitions(chess-console PRIVATE NO_GRAPHICS)

    # Also ensure ChessCore library gets the right definitions for console build
    if(NO_GRAPHICS)
        target_compile_definitions(chess-console PRIVATE NO_GRAPHICS)
    endif()
endif()

# Web version (to be compiled with Emscripten)
if(EMSCRIPTEN)
    file(GLOB_RECURSE WEB_SOURCES "src/web/*.cpp")
    add_executable(chess-web src/main.cc ${WEB_SOURCES})
    target_link_libraries(chess-web ChessCore)
    target_compile_definitions(chess-web PRIVATE WEB_VERSION NO_GRAPHICS)
endif()

# Enable testing
if(ENABLE_TESTING)
    enable_testing()
    
    # Test executable
    file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
    if(TEST_SOURCES)
        add_executable(chess-tests ${TEST_SOURCES})
        target_link_libraries(chess-tests ChessCore)
        add_test(NAME chess_unit_tests COMMAND chess-tests)
    endif()
endif()

# Installation
if(NO_GRAPHICS)
    install(TARGETS chess-console DESTINATION bin)
endif()
if(NOT NO_GRAPHICS AND X11_FOUND)
    install(TARGETS chess-desktop DESTINATION bin)
endif()

# Package configuration
set(CPACK_PACKAGE_NAME "ChessEngine")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Advanced Chess Engine with AI")
include(CPack)
