#include "rook.h"
#include "piece.h"
#include <vector>
using namespace std; 

// moves possible by a queen relative to its current position
vector<vector<int>> rookMoves = {
    {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0},
    {-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}, {-8, 0},
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8},
    {0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}, {0, -8}
};

Rook::Rook(bool isWhite, int row, int col): Piece{isWhite, rookMoves, 'r', row, col, 5} {}

bool Rook::checkValidMove(ChessBoard& cBoard, int toRow, int toCol) {
    int board_size = 8;
    if(toRow < 0 || toRow >= board_size || toCol < 0 || toCol >= board_size) {
        return false; 
    }
    
    // cannot move to its current square
    if ((toRow != row && toCol != col) || (toRow == row && toCol == col)) { return false; }
    
    // check the direction this piece is moving it: either moves horizontally or vertically
    if (toRow == row) { // horizontal move
        for (int i = 1; i < abs(toCol - col); ++i) {
            Piece* p = cBoard.getSquare(row, toCol > col ? col + i : col - i);
            if (p != nullptr) { return false; }
        }
    } else if (toCol == col) { // vertical move
        for (int i = 1; i < abs(toRow - row); ++i) {
            Piece* p = cBoard.getSquare(toRow > row ? row + i : row - i, col);
            if (p != nullptr) { return false; }
        }
    }

    // final check: if there exists a piece on the last square the piece moves to, it must capture it and that piece must be the opposite colour
    Piece* p = cBoard.getSquare(toRow, toCol);
    if (p != nullptr && p->getIsWhite() == this->getIsWhite()) { return false; }

    return true;
}
