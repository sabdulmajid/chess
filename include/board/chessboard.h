#ifndef CHESSBOARD_H
#define CHESSBOARD_H
#include <iostream>
#include <vector>
#include <memory>
#include "observer.h"
#include "textobserver.h"
#ifndef NO_GRAPHICS
#include "graphicalobserver.h"
#endif
#include "king.h"
#include "queen.h"
#include "rook.h"
#include "bishop.h"
#include "knight.h"
#include "pawn.h"
#include "piece.h"

class Observer;
class TextObserver;
#ifndef NO_GRAPHICS
class GraphicalObserver;
#endif

class ChessBoard {
    std::vector<Observer*> observers; // set of observers attached to chessboard subject
    std::vector<std::vector<Piece*>> board; // grid of board state

    std::vector<std::unique_ptr<Piece>> whitePieces; // set of all of white's pieces 
    std::vector<std::unique_ptr<Piece>> blackPieces; // set of all of black's pieces

    Piece* enPassantPawn; // TODO: MAKE SURE TO UPDATE THIS WHEN A PAWN MOVES TWO SQUARES

    public:
#ifndef NO_GRAPHICS
        ChessBoard(TextObserver *textDisplay, GraphicalObserver *graphicsDisplay); 
#else
        ChessBoard(TextObserver *textDisplay, void* unused = nullptr);
#endif 
        ChessBoard(const ChessBoard& other);
        virtual ~ChessBoard();

        void attach(Observer* o); // attach an observer
        void detach(Observer* o); // delete an observer
        void notifyObservers();  // notify observers

        bool checkIfPieceIsAttacked(Piece* piece, bool isWhite); // check if piece is being attacked on the board
        bool verifyMove(int fromRow, int fromCol, int toRow, int toCol, bool isWhite, char promotionType = ' '); // ` ` represents no promotion and verify if a move is viable

        bool checkIfKingIsInCheck(bool isWhite, int fromRow = -1, int fromCol = -1, int toRow = -1, int toCol = -1); // check if a king is in check after a move or currently
        bool checkCheckmate(bool isWhite);  // check if a colour has been mated
        bool checkStalemate(bool isWhite); // check if a colour is in stalemate
        bool checkNoPawnsInLastRank(); // check there are no pawns in the final rank

        Piece* getSquare(int row, int col) const; // get the piece at a row/col in the board
        void removeAllPieces(); // remove all pieces from board
        void placePiece(int row, int col, bool isWhite, char pieceType, bool moved = false); // place piece on the board 
        void removePiece(int row, int col); // remove piece at a row/col on the board
        void movePiece(int fromRow, int fromCol, int toRow, int toCol, char promotionType = 'x'); // moves a piece to a location, no checks done

        Piece* getKing(bool isWhite) const; // get location of a colour's king
        int getNumKings(bool isWhite) const; // get number of kings of a colour (to check board setup)
        Piece* getEnPassantPawn() const; // get the opponent's pawn that can be enpassanted 
        void setEnPassantPawn(Piece*); // set the pawn that can be enpassanted next turn
};

#endif
