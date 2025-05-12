#ifndef QUEEN_H
#define QUEEN_H
#include <vector>
#include <string>
#include "piece.h" 
#include "chessboard.h"

class Queen : public Piece {

    public:
        Queen(bool isWhite, int row, int col); 
        bool checkValidMove(ChessBoard& cBoard, int toRow, int toCol) override; // checks whether a move by a queen is valid
        virtual ~Queen() = default;

};

#endif
