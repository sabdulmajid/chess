#include "chessboard.h"
#include "piece.h"
#include <iostream> 
#include <vector> 
#include <cassert>
using namespace std; 

#ifndef NO_GRAPHICS
ChessBoard::ChessBoard(TextObserver* textDisplay, GraphicalObserver* graphicsDisplay): enPassantPawn{nullptr} {
    // initializes an 8x8 board
    for (int i = 0; i < 8; ++i) {
        vector<Piece*> row {8, nullptr};
        board.push_back(row);
    }

    // attaches the observers if they exist
    if (textDisplay != nullptr) attach(textDisplay);
    if (graphicsDisplay != nullptr) attach(graphicsDisplay);
}
#else
ChessBoard::ChessBoard(TextObserver* textDisplay, void* unused): enPassantPawn{nullptr} {
    // initializes an 8x8 board
    for (int i = 0; i < 8; ++i) {
        vector<Piece*> row {8, nullptr};
        board.push_back(row);
    }

    // attaches the text observer if it exists
    if (textDisplay != nullptr) attach(textDisplay);
}
#endif

// Copy constructor - useful for copying board states
ChessBoard::ChessBoard(const ChessBoard& other) {
    // initializes an 8x8 board
    for (int i = 0; i < 8; ++i) {
        vector<Piece*> row {8, nullptr};
        board.push_back(row);
    }

    // copies all white pieces to the new board
    for (int i = 0; i < other.whitePieces.size(); ++i) {
        Piece* p = other.whitePieces[i].get();
        placePiece(p->getRow(), p->getCol(), p->getIsWhite(), p->getPieceType(), p->getHasMoved());
    }

    // copies all white pieces to the new board
    for (int i = 0; i < other.blackPieces.size(); ++i) {
        Piece* p = other.blackPieces[i].get();
        placePiece(p->getRow(), p->getCol(), p->getIsWhite(), p->getPieceType(), p->getHasMoved());
    }
}

ChessBoard::~ChessBoard() {}

void ChessBoard::attach(Observer* o) {
    observers.emplace_back(o);
}

void ChessBoard::detach(Observer* o) {
    for (int i = 0; i < observers.size(); ++i) {
        if (observers[i] == o) {
            observers.erase(observers.begin() + i);
            break;
        }
    }
}

void ChessBoard::notifyObservers() {
    for (int i = 0; i < observers.size(); ++i) {
        observers[i]->notify(*this);
    }
}

Piece* ChessBoard::getSquare(int row, int col) const {
    return board[row][col];
}

// removes either a white or black piece from the board
void ChessBoard::removePiece(int row, int col) {
    Piece *p = getSquare(row, col);
    if (p == nullptr) { return; }

    // if a piece exists in the square, then remove it. Note: they are smart pointers, so we can just take them out of scope and they will delete themselves
    if (p->getIsWhite()) {
        for (int i = 0; i < whitePieces.size(); ++i) {
            Piece* tmpP = whitePieces[i].get();
            if (tmpP->getRow() == p->getRow() && tmpP->getCol() == p->getCol()) {
                whitePieces.erase(whitePieces.begin() + i);
                break;
            }
        }
    } else {
        for (int i = 0; i < blackPieces.size(); ++i) {
            Piece* tmpP = blackPieces[i].get();
            if (tmpP->getRow() == p->getRow() && tmpP->getCol() == p->getCol()) {
                blackPieces.erase(blackPieces.begin() + i);
                break;
            }
        }
    }

    // set the coordinate to nullptr to avoid invalid reads
    board[row][col] = nullptr;
}

// places a piece on the board
void ChessBoard::placePiece(int row, int col, bool isWhite, char pieceType, bool moved) {
    // remove a piece if it currently exists on the board
    if (getSquare(row, col) != nullptr) {
        removePiece(row, col);
    }

    // set the piece according to what type it is
    unique_ptr<Piece> p;
    if (pieceType == 'p') { p = make_unique<Pawn>(isWhite, row, col); } 
    else if (pieceType == 'r') { p = make_unique<Rook>(isWhite, row, col); } 
    else if (pieceType == 'n') { p = make_unique<Knight>(isWhite, row, col); } 
    else if (pieceType == 'b') { p = make_unique<Bishop>(isWhite, row, col); } 
    else if (pieceType == 'q') { p = make_unique<Queen>(isWhite, row, col); } 
    else if (pieceType == 'k') { p = make_unique<King>(isWhite, row, col); }

    // set the hasMoved field to account for board setups, pawn promotions, etc
    p.get()->setHasMoved(moved);

    // add the new piece into the array of white or black pieces
    board[row][col] = p.get();
    if (isWhite) {
        whitePieces.emplace_back(std::move(p));
    } else {
        blackPieces.emplace_back(std::move(p));
    }
}

// removes all the pieces from a board - used for setup purposes
void ChessBoard::removeAllPieces() {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            removePiece(i, j);
        }
    }
}

// retrieves the king of a certain colour by iterating through either the array of white pieces of black pieces
Piece* ChessBoard::getKing(bool isWhite) const {
    if (isWhite) {
        for (int i = 0; i < whitePieces.size(); ++i) {
            if (whitePieces[i].get()->getPieceType() == 'k') {
                return whitePieces[i].get();
            }
        }
    } else {
        for (int i = 0; i < blackPieces.size(); ++i) {
            if (blackPieces[i].get()->getPieceType() == 'k') {
                return blackPieces[i].get();
            }
        }
    }
    return nullptr;
}

// checks if a current piece exists from a square; if a pieceType and isWhite field is specified, it ensures that that piece is of the opposite colour of the colour specified
bool existsPieceInSquare(ChessBoard& board, int row, int col, char pieceType = ' ', bool isWhite = true) {
    if (row < 0 || row > 7 || col < 0 || col > 7) return false;
    if (pieceType == ' ') {
        return board.getSquare(row, col) != nullptr;
    }
    Piece* tmpPiece = board.getSquare(row, col);
    return (tmpPiece != nullptr && tmpPiece->getPieceType() == pieceType && tmpPiece->getIsWhite() != isWhite);
}

// checks if a current piece exists on a horizontal row
bool existsPieceInHorizontal(ChessBoard& board, int row, int col, char pieceType = ' ', bool isWhite = true) {
    int i = col;

    // check on the right of the current square
    while (i < 8) {
        if (existsPieceInSquare(board, row, i)) {
            if (pieceType != ' ' && existsPieceInSquare(board, row, i, pieceType, isWhite)) {
                return true;
            } else if(i != col) {
                break;
            }
        }
        i += 1;
    }

    // check on the left of the current square
    i = col;
    while (i >= 0) {
        if (existsPieceInSquare(board, row, i)) {
            if (pieceType != ' ' && existsPieceInSquare(board, row, i, pieceType, isWhite)) {
                return true;
            } else if(i != col) {
                break;
            }
        }
        i -= 1;
    }

    return false;
}

// checks if a current piece exists on a vertical row
bool existsPieceInVertical(ChessBoard& board, int row, int col, char pieceType = ' ', bool isWhite = true) {
    int i = row;

    // check on the top of the current square
    while (i < 8) {
        if (existsPieceInSquare(board, i, col)) {
            if (pieceType != ' ' && existsPieceInSquare(board, i, col, pieceType, isWhite)) {
                return true;
            } else if(i != row) {
                break;
            }
        }
        i += 1;
    }

    // check on the bottom of the current square
    i = row;
    while (i >= 0) {
        if (existsPieceInSquare(board, i, col)) {
            if (pieceType != ' ' && existsPieceInSquare(board, i, col, pieceType, isWhite)) {
                return true;
            } else if(i != row) {
                break;
            }
        }
        i -= 1;
    }

    return false;
}

// checks if a current piece exists on a diagonal
bool existsPieceInDiagonal(ChessBoard& board, int row, int col, char pieceType = ' ', bool isWhite = true) {
    int i = row;
    int j = col;

    // check in the top-right direction of the current square
    while (i < 8 && j < 8) {
        if (existsPieceInSquare(board, i, j)) {
            if (pieceType != ' ' && existsPieceInSquare(board, i, j, pieceType, isWhite)) {
                return true;
            } else if(i != row || j != col) {
                break;
            }
        }
        i += 1;
        j += 1;
    }

    // check in the bottom-left direction of the current square
    i = row;
    j = col;
    while (i >= 0 && j >= 0) {
        if (existsPieceInSquare(board, i, j)) {
            if (pieceType != ' ' && existsPieceInSquare(board, i, j, pieceType, isWhite)) {
                return true;
            } else if(i != row || j != col) {
                break;
            }
        }
        i -= 1;
        j -= 1;
    }

    // check in the top-left direction of the current square
    i = row;
    j = col;
    while (i < 8 && j >= 0) {
        if (existsPieceInSquare(board, i, j)) {
            if (pieceType != ' ' && existsPieceInSquare(board, i, j, pieceType, isWhite)) {
                return true;
            } else if(i != row || j != col) {
                break;
            }
        }
        i += 1;
        j -= 1;
    }

    // check in the bottom-right direction of the current square
    i = row;
    j = col;
    while (i >= 0 && j < 8) {
        if (existsPieceInSquare(board, i, j)) {
            if (pieceType != ' ' && existsPieceInSquare(board, i, j, pieceType, isWhite)) {
                return true;
            } else if(i != row || j != col) {
                break;
            }
        }
        i -= 1;
        j += 1;
    }

    return false;
}

// checks if any piece is attacked by any existing piece on the board
bool ChessBoard::checkIfPieceIsAttacked(Piece* piece, bool isWhite) {
    int row = piece->getRow();
    int col = piece->getCol();
    
    bool attackedByKing = existsPieceInSquare(*this, row+1, col+1, 'k', isWhite) || existsPieceInSquare(*this, row+1, col-1, 'k', isWhite) 
                        || existsPieceInSquare(*this, row-1, col+1, 'k', isWhite) || existsPieceInSquare(*this, row-1, col-1, 'k', isWhite) 
                        || existsPieceInSquare(*this, row+1, col, 'k', isWhite) || existsPieceInSquare(*this, row-1, col, 'k', isWhite) 
                        || existsPieceInSquare(*this, row, col+1, 'k', isWhite) || existsPieceInSquare(*this, row, col-1, 'k', isWhite);

    bool attackedByQueen = existsPieceInHorizontal(*this, row, col, 'q', isWhite) || existsPieceInVertical(*this, row, col, 'q', isWhite) || existsPieceInDiagonal(*this, row, col, 'q', isWhite);

    bool attackedByRook = existsPieceInHorizontal(*this, row, col, 'r', isWhite) || existsPieceInVertical(*this, row, col, 'r', isWhite);

    bool attackedByBishop = existsPieceInDiagonal(*this, row, col, 'b', isWhite);

    bool attackedByKnight = existsPieceInSquare(*this, row+2, col+1, 'n', isWhite) || existsPieceInSquare(*this, row+2, col-1, 'n', isWhite)
                        || existsPieceInSquare(*this, row-2, col+1, 'n', isWhite) || existsPieceInSquare(*this, row-2, col-1, 'n', isWhite) 
                        || existsPieceInSquare(*this, row+1, col+2, 'n', isWhite) || existsPieceInSquare(*this, row+1, col-2, 'n', isWhite) 
                        || existsPieceInSquare(*this, row-1, col+2, 'n', isWhite) || existsPieceInSquare(*this, row-1, col-2, 'n', isWhite);

    bool attackedByPawn = !isWhite ? existsPieceInSquare(*this, row-1, col-1, 'p', isWhite) || existsPieceInSquare(*this, row-1, col+1, 'p', isWhite)
                        : existsPieceInSquare(*this, row+1, col-1, 'p', isWhite) || existsPieceInSquare(*this, row+1, col+1, 'p', isWhite);
    
    bool attackedByEnPassantPawn = (piece->getPieceType() == 'p' && enPassantPawn != nullptr && ((col > 0 && getSquare(row, col-1) == enPassantPawn) || (col < 7 && getSquare(row, col+1) == enPassantPawn)));
    
    // return true if the current piece is attacked by any of the opponent's pieces
    return attackedByKing || attackedByQueen || attackedByRook || attackedByBishop || attackedByKnight || attackedByPawn || attackedByEnPassantPawn;
}


// checks if a king of a specific colour is currently in check - if coordinates are specified, then check that but after a hypothetical move
bool ChessBoard::checkIfKingIsInCheck(bool isWhite, int fromRow, int fromCol, int toRow, int toCol) {
    Piece* king = getKing(isWhite);
    
    // check if the king is still in check after a hypothetical move by generating a temporary new board
    if (fromRow != -1) {
        ChessBoard boardAfterMove = ChessBoard{*this};
        boardAfterMove.movePiece(fromRow, fromCol, toRow, toCol);
        Piece *newKing = boardAfterMove.getKing(isWhite); 
        return boardAfterMove.checkIfPieceIsAttacked(newKing, king->getIsWhite());
    }

    return checkIfPieceIsAttacked(king, king->getIsWhite());
}

// generate all possible moves allowed on the board by a current colour
vector<vector<int>> generateMoves(ChessBoard& cBoard, bool isWhite) {
    int board_size = 8; 
    vector<vector<int>> res; 

    // i and j represent the first square, while k and h represent the second square; if a piece exists in the first square and can move to the second error-free, then append it to the array of all possible moves
    for(int i = 0; i < board_size; i++) {
        for(int j = 0; j < board_size; j++) {
            // piece is nullptr
            if(cBoard.getSquare(i, j) == nullptr) continue;
            
            // piece is not our colour
            if(cBoard.getSquare(i, j)->getIsWhite() != isWhite) continue; 

            // square is our piece 
            for(int k = 0; k < board_size; k++) {
                for(int h = 0; h < board_size; h++) {
                    if(cBoard.verifyMove(i, j, k, h, isWhite)) {
                        // valid move
                        res.push_back({i, j, k, h});
                    }
                }
            }
        }
    }
    return res; 
}

// check if a current colour is in checkmate
bool ChessBoard::checkCheckmate(bool isWhite) {
    if(checkStalemate(isWhite) && checkIfKingIsInCheck(isWhite)) {
        return true; 
    }
    return false; 
}

// check if a current colour is in stalemate
bool ChessBoard::checkStalemate(bool isWhite) {
    vector<vector<int>> moves = generateMoves(*this, isWhite);
    if(moves.size() == 0) return true; 
    return false;
}

// check if pawns exist in the last rank (for board setup)
bool ChessBoard::checkNoPawnsInLastRank() {
    for (int i = 0; i < 8; ++i) {
        Piece* p = getSquare(0, i);
        if (p != nullptr && p->getPieceType() == 'p') { return true; }
        p = getSquare(7, i);
        if (p != nullptr && p->getPieceType() == 'p') { return true; }
    }
    return false;
}

// moves a piece from one square to another
void ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol, char promotionType) {
    Piece *p = getSquare(fromRow, fromCol);
    if (p == nullptr) { return; }

    // set en passant pawn
    if (p->getPieceType() == 'p' && abs(toRow - p->getRow()) == 2) {
        setEnPassantPawn(p);
    } else {
        setEnPassantPawn(nullptr);
    }

    // moving rook for castling
    if (p->getPieceType() == 'k' && abs(fromCol - toCol) == 2) {
        if (toCol == 6) {
            movePiece(fromRow, 7, fromRow, 5);
        } else {
            movePiece(fromRow, 0, fromRow, 3);
        }
    }

    // remove en passant pawn if it exists
    if (p->getPieceType() == 'p' && abs(toRow - p->getRow()) == abs(toCol - p->getCol()) && getSquare(toRow, toCol) == nullptr) {
        removePiece(p->getRow(), toCol);
    }

    // remove a piece in the current square if it exists, and set the piece in the the old square into the new square's location
    if (getSquare(toRow, toCol) != nullptr) {
        removePiece(toRow, toCol);
    }
    p->setCoords(toRow, toCol);
    board[toRow][toCol] = p;
    board[fromRow][fromCol] = nullptr;

    // pawn promotion checks
    if (p->getPieceType() == 'p' && (toRow == 0 || toRow == 7)) {

        promotionType = tolower(promotionType);

        if (promotionType == 'k') {
            cerr << "error: cannot promote pawn to king" << endl; 
        } else if(promotionType == 'x') {
            promotionType = 'q'; 
        }

        bool pIsWhite = p->getIsWhite();
        
        // if promotion is valid, then remove the current piece (which is a pawn) and place the new piece
        removePiece(toRow, toCol);

        placePiece(toRow, toCol, pIsWhite, promotionType, true);
    }

}


// verifies that a move is possible based on squares a piece is moving from and to
bool ChessBoard::verifyMove(int fromRow, int fromCol, int toRow, int toCol, bool isWhite, char promotionType) {
    if (fromRow < 0 || fromRow > 7 || fromCol < 0 || fromCol > 7 || toRow < 0 || toRow > 7 || toCol < 0 || toCol > 7) { return false; }

    Piece* piece = getSquare(fromRow, fromCol);
    if (piece == nullptr) { return false; }

    // check if a piece can move to the destination; no pieces block path; landing square is empty or occupied by DIFFERENT colour piece
    if (!(piece->checkValidMove(*this, toRow, toCol))) { return false; }

    // check that after the move, the king is not in check
    if (checkIfKingIsInCheck(isWhite, fromRow, fromCol, toRow, toCol)) { return false; }
    
    return true;
}


// returns the number of kings on the board (for board setup)
int ChessBoard::getNumKings(bool isWhite) const {
    int numKings = 0;
    if (isWhite) {
        for (int i = 0; i < whitePieces.size(); ++i) {
            if (whitePieces[i].get()->getPieceType() == 'k') {
                numKings++;
            }
        }
    } else {
        for (int i = 0; i < blackPieces.size(); ++i) {
            if (blackPieces[i].get()->getPieceType() == 'k') {
                numKings++;
            }
        }
    }
    return numKings;
}

Piece* ChessBoard::getEnPassantPawn() const {
    return enPassantPawn;
}

void ChessBoard::setEnPassantPawn(Piece* p) {
    enPassantPawn = p;
}
