#include "knight.h"
#include "piece.h"
#include <vector>
using namespace std; 

// set of all possible knight moves
vector<vector<int>> knightMoves = {
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
    {1, 2}, {-1, 2}, {1, -2}, {-1, -2},
};

Knight::Knight(bool isWhite, int row, int col): Piece{isWhite, knightMoves, 'n', row, col, 3} {}


bool Knight::checkValidMove(ChessBoard& board, int toRow, int toCol) {  
    int dy = toRow - row;
    int dx = toCol - col;
    int board_size = 8;

    // cehck if move is in the board
    if(toRow < 0 || toRow >= board_size || toCol < 0 || toCol >= board_size) {
        return false; 
    }

    // check if move is a possible knight move
    bool isKnightMove = false; 
    for(int i = 0; i < moves.size(); i++) {
        if(moves[i][0] == dx && moves[i][1] == dy) {
            // exists in possible knight moves
            isKnightMove = true; 
            break; 
        }
    }

    if(!isKnightMove) {
        // not a possible knight move
        return false; 
    }

    // check if moving to capture own piece
    if(board.getSquare(toRow, toCol) != nullptr && board.getSquare(toRow, toCol)->getIsWhite() == getIsWhite()) {
        // our own piece is already on the square, not possible
        return false; 
    }

    return true; 
}
