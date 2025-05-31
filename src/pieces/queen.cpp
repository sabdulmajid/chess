#include "queen.h"
#include "piece.h"
#include <vector>
using namespace std; 

// moves possible by a queen relative to its current position
vector<vector<int>> queenMoves = {
    // horizontal moves
    {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0},
    {-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}, {-8, 0},
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8},
    {0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}, {0, -8},

    // diagonal moves
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, 
    {2, 2}, {2, -2}, {-2, 2}, {-2, -2},
    {3, 3}, {3, -3}, {-3, 3}, {-3, -3},
    {4, 4}, {4, -4}, {-4, 4}, {-4, -4},
    {5, 5}, {5, -5}, {-5, 5}, {-5, -5},
    {6, 6}, {6, -6}, {-6, 6}, {-6, -6},
    {7, 7}, {7, -7}, {-7, 7}, {-7, -7},
    {8, 8}, {8, -8}, {-8, 8}, {-8, -8}
};

Queen::Queen(bool isWhite, int row, int col): Piece{isWhite, queenMoves, 'q', row, col, 9} {}

bool Queen::checkValidMove(ChessBoard& cBoard, int toRow, int toCol) {
    int board_size = 8;
    if (toRow < 0 || toRow >= board_size || toCol < 0 || toCol >= board_size) {
        return false; 
    }
    
    // cannot move to its current square
    if (toRow == row && toCol == col) { return false; }
    
    // check the direction this piece is moving it: either moves horizontally/vertically or diagonally
    if (toRow == row || toCol == col) {
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
    } else if (abs(toRow - row) == abs(toCol - col)) { // diagonal move
        for (int i = 1; i < abs(toRow - row); ++i) {
            Piece* p = cBoard.getSquare(toRow > row ? row + i : row - i, toCol > col ? col + i: col - i);
            if (p != nullptr) { return false; }
        }
    } else {
        return false;
    }

    // final check: if there exists a piece on the last square the piece moves to, it must capture it and that piece must be the opposite colour
    Piece* p = cBoard.getSquare(toRow, toCol);
    if (p != nullptr && p->getIsWhite() == this->getIsWhite()) { return false; }

    return true;
}
