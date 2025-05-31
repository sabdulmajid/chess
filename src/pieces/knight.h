#ifndef KNIGHT_H
#define KNIGHT_H
#include <vector>
#include <string>
#include "piece.h" 
#include "chessboard.h"

class Knight : public Piece {

    public:
        Knight(bool isWhite, int row, int col); 
        bool checkValidMove(ChessBoard& board, int toRow, int toCol) override; // checks whether a move by a knight is valid
        virtual ~Knight() = default;

};

#endif
