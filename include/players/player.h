#ifndef PLAYER_H
#define PLAYER_H
#include <vector>
#include <string> 
#include <iostream>
#include <sstream>
#include "chessboard.h"

class Player {

    protected:
        bool isWhite; // colour of the player
        std::istream &in = std::cin; //input stream
        std::ostream &out = std::cout; // output stream

    public:
        Player(bool isWhite); 
        virtual bool makeMove(ChessBoard& board) = 0; // generates move from player
        virtual ~Player() = default;

};

#endif
