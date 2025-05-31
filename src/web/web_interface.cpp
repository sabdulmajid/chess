#include "web_interface.h"
#include <sstream>
#include <algorithm>
// Note: Simplified version without jsoncpp for WebAssembly compatibility

WebInterface::WebInterface() : game(nullptr) {
    // Initialize game will be called from startNewGame
}

WebInterface::~WebInterface() = default;

void WebInterface::startNewGame(bool whiteIsHuman, bool blackIsHuman, int difficulty) {
    // Create a new game instance
    game = std::make_unique<Game>(nullptr); // No X11 window for web version
    game->startGame(whiteIsHuman, blackIsHuman, difficulty, difficulty);
    
    if (gameStateCallback) {
        gameStateCallback(getGameStatus());
    }
}

void WebInterface::makeMove(const std::string& from, const std::string& to, const std::string& promotion) {
    if (!game) return;

    auto fromPos = parsePosition(from);
    auto toPos = parsePosition(to);
    
    // Convert to the game's expected format and make the move
    // This would need to interface with the existing game logic
    
    if (updateCallback) {
        updateCallback(getBoardState());
    }
    
    if (gameStateCallback) {
        gameStateCallback(getGameStatus());
    }
}

void WebInterface::resign() {
    if (!game) return;
    
    // Handle resignation logic
    if (gameStateCallback) {
        gameStateCallback("{\"status\":\"resigned\"}");
    }
}

void WebInterface::resetGame() {
    game.reset();
    if (updateCallback) {
        updateCallback("{}"); // Empty board
    }
}

std::string WebInterface::getBoardState() const {
    if (!game) return "{}";
    
    std::stringstream ss;
    ss << "{\"board\":[";
    
    // Convert the internal board state to simple JSON string
    for (int row = 0; row < 8; ++row) {
        if (row > 0) ss << ",";
        ss << "[";
        for (int col = 0; col < 8; ++col) {
            if (col > 0) ss << ",";
            // This would need to access the game's board state
            // For now, return empty squares
            ss << "{\"piece\":\"\",\"color\":\"\",\"position\":\"" 
               << positionToString(row, col) << "\"}";
        }
        ss << "]";
    }
    
    ss << "],\"turn\":\"white\"}";
    return ss.str();
}

std::string WebInterface::getGameStatus() const {
    if (!game) return "{\"status\":\"no_game\"}";
    
    std::stringstream ss;
    ss << "{\"status\":\"playing\",\"turn\":\"white\","
       << "\"check\":false,\"checkmate\":false,\"stalemate\":false}";
    return ss.str();
}

std::string WebInterface::getPossibleMoves(const std::string& position) const {
    if (!game) return "[]";
    
    auto pos = parsePosition(position);
    // Generate possible moves for the piece at this position
    // This would interface with the existing move generation logic
    return "[]"; // Placeholder - empty moves array
}

bool WebInterface::isValidMove(const std::string& from, const std::string& to) const {
    if (!game) return false;
    
    auto fromPos = parsePosition(from);
    auto toPos = parsePosition(to);
    
    // Check if the move is valid using existing game logic
    return true; // Placeholder
}

void WebInterface::setUpdateCallback(std::function<void(const std::string&)> callback) {
    updateCallback = callback;
}

void WebInterface::setGameStateCallback(std::function<void(const std::string&)> callback) {
    gameStateCallback = callback;
}

std::pair<int, int> WebInterface::parsePosition(const std::string& pos) {
    if (pos.length() != 2) return {-1, -1};
    
    int col = tolower(pos[0]) - 'a';
    int row = pos[1] - '1';
    
    if (col < 0 || col > 7 || row < 0 || row > 7) {
        return {-1, -1};
    }
    
    return {row, col};
}

std::string WebInterface::positionToString(int row, int col) {
    if (row < 0 || row > 7 || col < 0 || col > 7) {
        return "";
    }
    
    std::string result;
    result += static_cast<char>('a' + col);
    result += static_cast<char>('1' + row);
    return result;
}
