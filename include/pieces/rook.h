#ifndef ROOK_H
#define ROOK_H
#include <vector>
#include <string>
#include "piece.h" 
#include "chessboard.h"

class Rook : public Piece {

    public:
        Rook(bool isWhite, int row, int col); 
        bool checkValidMove(ChessBoard& cBoard, int toRow, int toCol) override; // checks whether a move by a rook is valid
        virtual ~Rook() = default;
};

#endif
