#ifndef COMPUTER_H
#define COMPUTER_H
#include <vector>
#include <string> 
#include "player.h"
#include "chessboard.h"

class Computer : public Player {

    int difficulty;
    std::vector<std::vector<int> > generateMoves(ChessBoard& board); // generate all possible moves the computer can make

    bool makeMove1(ChessBoard& cBoard);  // generate move of difficulty 1
    bool makeMove2(ChessBoard& cBoard);  // generate a move of difficulty 2
    bool makeMove3(ChessBoard& cBoard);  // generate a move of difficulty 3
    bool makeMove4(ChessBoard& cBoard);  // generate a move of difficulty 4

    public:
        Computer(bool isWhite, int difficulty); 
        bool makeMove(ChessBoard& board) override; // generate a computer move
        virtual ~Computer() = default;

};

#endif
