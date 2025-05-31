#ifndef GAME_H
#define GAME_H
#include <vector>
#include <iostream>
#include "player.h"
#include "human.h"
#include "computer.h"
#include "chessboard.h"
#include "textobserver.h"

#ifndef NO_GRAPHICS
#include "graphics/window.h"
#include "graphicalobserver.h"
#endif

class Player;

class Game {
    std::unique_ptr<Player> pWhite;
    std::unique_ptr<Player> pBlack;
    float scoreWhite;
    float scoreBlack;

    // Displays
    std::unique_ptr<TextObserver> textDisplay;
#ifndef NO_GRAPHICS
    std::unique_ptr<GraphicalObserver> graphicalDisplay;
#endif

    std::unique_ptr<ChessBoard> board; 
    bool isWhiteTurn;
    bool setupMode;

    // IO streams
    std::istream &in = std::cin;
    std::ostream &out = std::cout;

    public:
#ifndef NO_GRAPHICS
        Game(Xwindow* window);
#else
        Game(void* unused = nullptr);
#endif

        void startGame(bool whiteIsHuman, bool blackIsHuman, int whiteDifficulty, int blackDifficulty);
        void setupNormalBoard();
        void setupBoard();  
        void renderScore() const; 
        bool runTurn(); 

        virtual ~Game();

};

#endif
