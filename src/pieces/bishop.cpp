#include "bishop.h"
#include "piece.h"
#include <vector>
using namespace std; 

// moves possible by a queen relative to its current position
vector<vector<int>> bishopMoves = {
    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}, 
    {2, 2}, {2, -2}, {-2, 2}, {-2, -2},
    {3, 3}, {3, -3}, {-3, 3}, {-3, -3},
    {4, 4}, {4, -4}, {-4, 4}, {-4, -4},
    {5, 5}, {5, -5}, {-5, 5}, {-5, -5},
    {6, 6}, {6, -6}, {-6, 6}, {-6, -6},
    {7, 7}, {7, -7}, {-7, 7}, {-7, -7},
    {8, 8}, {8, -8}, {-8, 8}, {-8, -8},
};

Bishop::Bishop(bool isWhite, int row, int col): Piece{isWhite, bishopMoves, 'b', row, col, 3} {}

bool Bishop::checkValidMove(ChessBoard& cBoard, int toRow, int toCol) {
    int board_size = 8;
    if(toRow < 0 || toRow >= board_size || toCol < 0 || toCol >= board_size) {
        return false; 
    }
    
    // cannot move to its current square
    if (toRow == row && toCol == col) { return false; }

    // the move must be a diagonal move, so the absolute value of the difference in x must equal the absolute value of the difference in y
    if (abs(toRow - row) != abs(toCol - col)) { return false; }
    for (int i = 1; i < abs(toRow - row); ++i) {
        Piece* p = cBoard.getSquare(toRow > row ? row + i : row - i, toCol > col ? col + i: col - i);
        if (p != nullptr) { return false; }
    }

    // final check: if there exists a piece on the last square the piece moves to, it must capture it and that piece must be the opposite colour
    Piece* p = cBoard.getSquare(toRow, toCol);
    if (p != nullptr && p->getIsWhite() == this->getIsWhite()) { return false; }

    return true;
}
