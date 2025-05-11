#include "pawn.h"
#include "piece.h"
#include <vector>
using namespace std; 


// set of possible pawn moves for white
vector<vector<int>> whitePawnMoves = {
    {0, 1}, {1, 1}, {-1, 1}, {0, 2}
};

// set of possible pawn moves for black
vector<vector<int>> blackPawnMoves = {
    {0, -1}, {1, -1}, {-1, -1}, {0, -2}
};

Pawn::Pawn(bool isWhite, int row, int col): Piece{isWhite, isWhite ? whitePawnMoves : blackPawnMoves, 'p', row, col, 1} {}

bool Pawn::checkValidMove(ChessBoard& cBoard, int toRow, int toCol) {
    int dy = toRow - row;
    int dx = toCol - col;

    int board_size = 8;

    // check if move is in the board
    if(toRow < 0 || toRow >= board_size || toCol < 0 || toCol >= board_size) {
        return false; 
    }

    // check if move is in the set of possible pawn moves
    bool isPawnMove = false; 
    for(int i = 0; i < moves.size(); i++) {
        if(moves[i][0] == dx && moves[i][1] == dy) {
            // it is in the set of possible pawn moves!
            isPawnMove = true; 
            break; 
        }
    }

    // not a pawn move, not a valid move
    if(!isPawnMove) {
        return false; 
    }

    // check if pawn is moving by 2 squares or 1 
    if( (dy == 2 || dy == -2)) {
        if(getHasMoved() == true) {
            // has moved so can't move two squares
            return false; 
        } else {
            // check if there is a piece blocking the squares
            if (cBoard.getSquare(toRow, toCol) != nullptr) {
                return false; 
            }

            if (cBoard.getSquare(row + dy/2, toCol) != nullptr) { 
                return false; 
            }
        }
    } else if (dx != 0) {
        // pawn is moving horizontally and is taking another piece
        if(cBoard.getSquare(toRow, toCol) == nullptr) {
            // en passant or invalid move
            if (cBoard.getSquare(this->row, toCol) == nullptr) { return false; } 
            else if (cBoard.getSquare(this->row, toCol) != cBoard.getEnPassantPawn()) { return false; } // correct since the en passant pawn is never your own pawn
        } else if(cBoard.getSquare(toRow, toCol)->getIsWhite() == getIsWhite()) { 
            // trying to take piece with same colour
            return false; 
        }
        
    } else {
        // is moving one square up 
        if(cBoard.getSquare(toRow, toCol) != nullptr) {
            // trying to move pawn into a square with an existing piece 
            return false; 
        }
    }   
    
    return true;
}
