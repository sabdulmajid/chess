#ifndef BISHOP_H
#define BISHOP_H
#include <vector>
#include <string> 
#include "piece.h"
#include "chessboard.h"

class Bishop : public Piece {

    public:
        Bishop(bool isWhite, int row, int col); 
        bool checkValidMove(ChessBoard& cBoard, int toRow, int toCol) override; // checks whether a move by a bishop is valid
        virtual ~Bishop() = default;
};

#endif
