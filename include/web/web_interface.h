#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <string>
#include <functional>
#include <memory>
#include "chessboard.h"
#include "game.h"

#ifdef WEB_VERSION
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#endif

class WebInterface {
private:
    std::unique_ptr<Game> game;
    std::function<void(const std::string&)> updateCallback;
    std::function<void(const std::string&)> gameStateCallback;

public:
    WebInterface();
    ~WebInterface();

    // Game control methods
    void startNewGame(bool whiteIsHuman, bool blackIsHuman, int difficulty = 2);
    void makeMove(const std::string& from, const std::string& to, const std::string& promotion = "");
    void resign();
    void resetGame();

    // Game state methods
    std::string getBoardState() const;
    std::string getGameStatus() const;
    std::string getPossibleMoves(const std::string& position) const;
    bool isValidMove(const std::string& from, const std::string& to) const;

    // Callback setters
    void setUpdateCallback(std::function<void(const std::string&)> callback);
    void setGameStateCallback(std::function<void(const std::string&)> callback);

    // Utility methods
    static std::pair<int, int> parsePosition(const std::string& pos);
    static std::string positionToString(int row, int col);
};

#ifdef WEB_VERSION
// Emscripten bindings
EMSCRIPTEN_BINDINGS(chess_module) {
    emscripten::class_<WebInterface>("WebInterface")
        .constructor()
        .function("startNewGame", &WebInterface::startNewGame)
        .function("makeMove", &WebInterface::makeMove)
        .function("resign", &WebInterface::resign)
        .function("resetGame", &WebInterface::resetGame)
        .function("getBoardState", &WebInterface::getBoardState)
        .function("getGameStatus", &WebInterface::getGameStatus)
        .function("getPossibleMoves", &WebInterface::getPossibleMoves)
        .function("isValidMove", &WebInterface::isValidMove);
}
#endif

#endif // WEB_INTERFACE_H
