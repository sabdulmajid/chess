#include "piece.h"
using namespace std;

Piece::Piece(bool isWhite, vector<vector<int>> moves, char pieceType, int row, int col, int value)
    : isWhite{isWhite}, moves{moves}, pieceType{pieceType}, row{row}, col{col}, hasMoved{false}, value{value} {}

Piece::~Piece() {}

// getter function for whether piece is white
bool Piece::getIsWhite() const {
    return isWhite;
}

// getter for the type of the piece
char Piece::getPieceType() const {
    return pieceType;
}

// get whether piece has moved
bool Piece::getHasMoved() const {
    return hasMoved;
}

// get row of piece
int Piece::getRow() const {
    return row;
}

// get column of piece
int Piece::getCol() const {
    return col;
}

// set whether piece has moved
void Piece::setHasMoved(bool hasMoved) {
    this->hasMoved = hasMoved;
}

// set row and column of piece
void Piece::setCoords(int row, int col) {
    this->row = row;
    this->col = col;
}

// get point value of piece
int Piece::getValue() const {
    return value; 
}
