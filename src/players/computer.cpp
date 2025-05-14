#include "computer.h"
#include "piece.h"
using namespace std;

Computer::Computer(bool isWhite, int difficulty): Player{isWhite}, difficulty{difficulty} {}

// generates set of all possible legal moves at this point 
vector<vector<int>> Computer::generateMoves(ChessBoard& cBoard) {
    int board_size = 8; 
    
    string playerColour = isWhite ? "white" : "black";
    vector<vector<int>> res; 

    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            // iterate through all starting move squares and check there is a piece and that it is our piece
            if (cBoard.getSquare(i, j) == nullptr) continue; 
            if (cBoard.getSquare(i, j)->getIsWhite() != isWhite) continue; 

            // piece is computer's piece and so we must check ending squares
            for (int k = 0; k < board_size; k++) {
                for (int h = 0; h < board_size; h++) {
                    // iterate through all possible ending squares
                    if (cBoard.verifyMove(i, j, k, h, isWhite)) {
                        // check if board classifies move as viable, if so add to set of moves
                        res.push_back({i, j, k, h});
                    }
                }
            }
        }
    }
    return res; 
}

// executes a move of difficulty level 1
bool Computer::makeMove1(ChessBoard& cBoard) {
    int board_size = 8; 

    // calculate all possible moves we can make
    vector<vector<int>> moves = generateMoves(cBoard);

    if (moves.size() == 0) return false; // cannot generate a viable move

    // pick the first move
    vector<int> move = moves[0]; 

    // check if move is a pawn promotion, if so promote to queen
    if (cBoard.getSquare(move[0], move[1])->getPieceType() == 'p' && (move[2] == board_size - 1 || move[2] == 0) ) {
        cBoard.movePiece(move[0], move[1], move[2], move[3], 'q');
    } else {
        // not pawn promotion, just move normally
        cBoard.movePiece(move[0], move[1], move[2], move[3], 'x');
    }

    // return true as we made a move
    return true; 
}

// executes a move of difficulty level 2
bool Computer::makeMove2(ChessBoard& cBoard) {
    int board_size = 8; 
    
    // get set of all possible moves 
    vector<vector<int> > moves = generateMoves(cBoard); 

    // no possible move, so we fail
    if (moves.size() == 0) return false;

    for (int i = 0; i < moves.size(); i++) {
        // pick a move
        vector<int> move = moves[i];

        // if the move does not take a piece or the piece colour, don't consider this 
        if (cBoard.getSquare(move[2], move[3]) == nullptr) continue; 
        if (cBoard.getSquare(move[2], move[3])->getIsWhite() != isWhite) {
            // move takes an opponent's piece
            if (cBoard.getSquare(move[0], move[1])->getPieceType() == 'p' && (move[2] == board_size - 1 || move[2] == 0) ) {
                // capture with promotion on pawn
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'q');
                return true;
            } else {
                // regular capturing move
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'x');
                return true;
            }
        }
    }

    // move results in check and does not take a piece
    for(int i = 0; i < moves.size(); i++) {
        // iterate through all moves and pick one
        vector<int> move = moves[i];

        // board copy for simulation purposes
        ChessBoard boardAfterMove = ChessBoard(cBoard); 

        // promotion on pawn
        if (cBoard.getSquare(move[0], move[1])->getPieceType() == 'p' && (move[2] == board_size - 1 || move[2] == 0)) {
            // move is a pawn promotion 
            boardAfterMove.movePiece(move[0], move[1], move[2], move[3], 'q');

            if(boardAfterMove.checkIfKingIsInCheck(!isWhite)) {
                // if the enemy king is in check, execute the move on the actual board
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'q');
                return true; 
            } 
        } else {
            // move is a regular move
            boardAfterMove.movePiece(move[0], move[1], move[2], move[3], 'x');
            
            if (boardAfterMove.checkIfKingIsInCheck(!isWhite)) {
                // if the enemy king is in check, execute the move on the actual board
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'x');
                return true; 
            } 
        }
    }

    // no checks and takes could be made so proceed to difficulty 1 
    return makeMove1(cBoard);
}

// generates a move of difficulty level 3
bool Computer::makeMove3(ChessBoard& cBoard) {
    int board_size = 8;

    // get set of all possible moves
    vector<vector<int> > moves = generateMoves(cBoard);

    // no moves can be made
    if (moves.size() == 0) return false;

    // avoid capture 
    for (int i = 0; i < moves.size(); i++) {
        vector<int> move = moves[i];
        // iterate through all moves and select one 

        Piece* currentPiece = cBoard.getSquare(move[0], move[1]);

        // if the current piece is not attacked, we don't care        
        if(!cBoard.checkIfPieceIsAttacked(currentPiece, isWhite)) continue; 

        // board to simulate moves
        ChessBoard boardAfterMove = ChessBoard(cBoard); 

        if (cBoard.getSquare(move[0], move[1])->getPieceType() == 'p' && (move[2] == board_size - 1 || move[2] == 0) ) {
            // move is a pawn promotion 
            boardAfterMove.movePiece(move[0], move[1], move[2], move[3], 'q');

            if(!boardAfterMove.checkIfPieceIsAttacked(boardAfterMove.getSquare(move[2], move[3]), isWhite)) {
                // piece is not attacked anymore after moving, so execute the move
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'q');
                return true; 
            } 
        } else {
            // regular move
            boardAfterMove.movePiece(move[0], move[1], move[2], move[3]);

            if(!boardAfterMove.checkIfPieceIsAttacked(boardAfterMove.getSquare(move[2], move[3]), isWhite)) {
                // piece is not attacked anymore after moving, so execute the move
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'x');
                return true;
            } 
        }
    }
    
    // no difficulty level 3 move can be made, so proceed to difficulty level 2
    return makeMove2(cBoard); 
}

bool Computer::makeMove4(ChessBoard& cBoard) {
    int board_size = 8;

    // generate the set of all possible moves
    vector<vector<int>> moves = generateMoves(cBoard);

    for(int i = 0; i < moves.size(); i++) {
        // iterate through moves and select one
        vector<int> move = moves[i];

        // get the current piece and it's numeric value
        Piece* currentPiece = cBoard.getSquare(move[0], move[1]);
        int curVal = currentPiece->getValue(); 

        // check piece on the square being moved to
        Piece* capturePiece = cBoard.getSquare(move[2], move[3]);

        // check if move is capture, if not we don't care
        if(capturePiece == nullptr)  continue; 

        // get value of piece we are capturing
        int captureVal = capturePiece->getValue(); 

        if(captureVal > curVal || (captureVal == curVal && isWhite)) {
            // good capture because piece is worth more
            if(currentPiece->getPieceType() == 'p' && (move[2] == board_size - 1 || move[2] == 0)) {
                // pawn promotion, execute move
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'q');
            } else {
                // execute move
                cBoard.movePiece(move[0], move[1], move[2], move[3]);
            }
            return true; 
        }

        // board to simulate the move, this is to make sure there is a free capture and no trading
        ChessBoard boardAfterMove = ChessBoard(cBoard); 

        // simulate the emove
        if(currentPiece->getPieceType() == 'p' && (move[2] == board_size - 1 || move[2] == 0)) {
            boardAfterMove.movePiece(move[0], move[1], move[2], move[3], 'q');
        } else {
            boardAfterMove.movePiece(move[0], move[1], move[2], move[3]);
        }

        // get the piece after we move 
        Piece *pieceAfterMove = boardAfterMove.getSquare(move[2], move[3]);
        if(!boardAfterMove.checkIfPieceIsAttacked(pieceAfterMove, isWhite)) {
            // capture with no recapture as our piece is not attacked afterwards, so execute moves
            if(currentPiece->getPieceType() == 'p' && (move[2] == board_size - 1 || move[2] == 0)) {
                cBoard.movePiece(move[0], move[1], move[2], move[3], 'q');
            } else {
                cBoard.movePiece(move[0], move[1], move[2], move[3]);
            }
            return true;
        }
    }

    // there are no viable difficulty 4 level moves, so proceed to difficulty 3
    return makeMove3(cBoard);
}

// function to generate move based on difficulty level
bool Computer::makeMove(ChessBoard& cBoard) {

    string inputLine; 

    while(getline(in, inputLine)) {
        istringstream iss{inputLine}; 
        string command; 
        iss >> command; // read in commands

        if (command == "resign") {
            // resign the computer, return false to indicate no moves made
            return false;
        } else if (command == "move") {
            // execute moves based on difficulty
            if (difficulty == 1) {
                // find a piece that is on the board and randomly choose a valid move for that piece
                return makeMove1(cBoard);
            } else if(difficulty == 2) {
                // prefers capturing moves and checks
                return makeMove2(cBoard);
            } else if(difficulty == 3) {
                // prefers avoiding capture, capturing moves, and checks
                return makeMove3(cBoard);         
            } else if(difficulty == 4) {
                // captures free pieces or pieces weighted more than another piece
                return makeMove4(cBoard);
            } 
        } else {
            cerr << "Invalid computer command" <<endl;
        }
    }
    
    return true;
}
