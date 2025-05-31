#ifndef KING_H
#define KING_H
#include <vector>
#include <string>
#include "piece.h" 
#include "chessboard.h"

class King : public Piece {

    public:
        King(bool isWhite, int row, int col);
        bool checkValidMove(ChessBoard& board, int toRow, int toCol) override; // checks whether a move by a king is valid
        virtual ~King() = default;

};

#endif
