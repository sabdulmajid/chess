#ifndef PIECE_H
#define PIECE_H
#include <vector>
#include <string>

class ChessBoard;

class Piece {
protected:
    bool isWhite; // the colour of a piece
    std::vector<std::vector<int>> moves; // set of all moves a piece can make assuming board state does not matter
    char pieceType; // type of a piece
    int row, col; // row and column of a piece
    bool hasMoved; // whether a piece has moved or not
    int value; // point value of a piece

public:
    Piece(bool isWhite, std::vector<std::vector<int>> moves, char pieceType, int row, int col, int value);
    virtual ~Piece();

    virtual bool checkValidMove(ChessBoard& board, int toRow, int toCol) = 0; // check if move is a viable move for the piece

    bool getIsWhite() const; // get colour of the piece
    char getPieceType() const; // get type of piece
    bool getHasMoved() const; // get whether piece has moved
    int getRow() const; // get row of piece
    int getCol() const; // get column of piece
    int getValue() const; // get value of piece
    void setHasMoved(bool hasMoved); //set whether pieice has moved
    void setCoords(int row, int col); //set row/col of a piece
};

#endif
