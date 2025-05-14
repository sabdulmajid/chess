#ifndef HUMAN_H
#define HUMAN_H
#include <vector>
#include <string> 
#include "player.h"
#include "chessboard.h"

class Human : public Player {
    public:
        Human(bool isWhite); 
        bool makeMove(ChessBoard& board) override; // get move from human
        virtual ~Human() = default;

};

#endif
