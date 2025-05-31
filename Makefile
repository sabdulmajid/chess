# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++14

# Linker flags
LDFLAGS = -lX11

# Source files
SRC = piece.cpp bishop.cpp chessboard.cpp computer.cpp game.cpp graphicalobserver.cpp \
      human.cpp king.cpp knight.cpp pawn.cpp \
      player.cpp queen.cpp rook.cpp textobserver.cpp window.cpp

# Header files
HEADERS = piece.h bishop.h chessboard.h computer.h game.h graphicalobserver.h \
          human.h king.h knight.h observer.h pawn.h player.h \
          queen.h rook.h textobserver.h window.h

# Object files
OBJ = $(SRC:.cpp=.o)

# Executable name
EXEC = chess

# Default target
all: $(EXEC)

# Linking
$(EXEC): $(OBJ) main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilation of main.o
main.o: main.cc
	$(CXX) $(CXXFLAGS) -c main.cc -o main.o

# Clean target
clean:
	rm -f $(OBJ) $(EXEC) main.o

.PHONY: all clean
