#include "human.h"
#include "piece.h"
using namespace std;

Human::Human(bool isWhite): Player{isWhite} {}

// function to get move from a human
bool Human::makeMove(ChessBoard& board) {
    string inputLine;

    while(getline(in, inputLine)) {
        istringstream iss{inputLine};
        string command;
        iss >> command;
        // read in human's command
        if (command == "resign") {
            // if resign, return false to indicate no move and resignation
            return false;
        } else if (command == "move") {
            char startCol;
            int startRow;
            char endCol;
            int endRow;
            char promotionType = ' ';

            // read in move information
            iss >> startCol >> startRow >> endCol >> endRow;

            // reindex rows to 0 indexed
            startRow--; 
            endRow--;

            // convert columns to numeric equivalence
            int startColNum = int(startCol - 'a'); 
            int endColNum = int(endCol - 'a'); 

            if (!iss) { cerr << "Incorrect input. " << endl; continue; }


            if (startRow >= 0 && startRow < 8 && startCol >= 'a' && startCol <= 'h' && endRow >= 0 && endRow < 8 && endCol >= 'a' && endCol <= 'h') {
                // make sure rows and columns are in the board 
                iss >> promotionType;

                if (iss) { // check if there the move is a promotion
                    char lowercasePromotionType = tolower(promotionType);

                    if (lowercasePromotionType == 'r' || lowercasePromotionType == 'n' || lowercasePromotionType == 'b' || lowercasePromotionType == 'q') {
                        // check if the promotion is a viable piece

                        // get the piece that is being moved and promoted
                        Piece* piece = board.getSquare(startRow, startColNum);

                        // check if piece exists, is our piece and is a pawn
                        if (piece == nullptr) { cerr << "No piece at start position in Human::makeMove: " << startCol << startRow + 1 << " " << endCol << endRow + 1 << endl; }
                        else if (piece->getIsWhite() != isWhite) { cerr << "Invalid piece at start position in Human::makeMove: " << startCol << startRow + 1 << " " << endCol << endRow + 1 << endl; } 
                        else if (piece->getPieceType() != 'p') { cerr << "Invalid piece type at start position in Human::makeMove: " << startCol << startRow + 1 << " " << endCol << endRow + 1 << endl; }
                        else {
                            // the piece is a viable piece to be promoted, do more speciffic checks
                            if (board.verifyMove(startRow, startColNum, endRow, endColNum, isWhite, promotionType)) {
                                // verify that the move is possible if it is, we execute the move
                                board.movePiece(startRow, startColNum, endRow, endColNum, promotionType);
                                return true;
                            } else {
                                // move is not a valid move
                                cerr << "Invalid move in Human::makeMove: " << startCol << startRow + 1 << " " << endCol << endRow + 1 << endl;
                            }
                        }
                    } else {
                        // promotion type is not a valid piece
                        cerr << "Invalid promotion type in Human::makeMove: " << startCol << startRow + 1 << " " << endCol << endRow + 1 << endl;
                    }
                } else { // move is not a promotion
                    Piece* piece = board.getSquare(startRow, startColNum);
                    
                    // get the piece we are trying to move and make sure it exists and is our piece
                    if (piece == nullptr) { cerr << "No piece at start position in Human::makeMove: " << startCol << startRow + 1 << " " << endCol << endRow + 1 << endl; }
                    else if (piece->getIsWhite() ^ isWhite) { cerr << "Invalid piece at start position in Human::makeMove: " << startCol << startRow + 1 << " " << endCol << endRow + 1 << endl; }
                    else {
                        // is a valid piece to move
                        if (board.verifyMove(startRow, startColNum, endRow, endColNum, isWhite)) {
                            // is a valid move, so execute move
                            board.movePiece(startRow, startColNum, endRow, endColNum);
                            return true;
                        } else {
                            // not a valid move
                            cerr << "Invalid move in Human::makeMove" << endl;
                        }
                    }
                }
            } else {
                cerr << "Invalid move in Human::makeMove" << endl;
            }
        } else {
            cerr << "Invalid command in Human::makeMove" << endl;
        }
    }
    
    return true; // this should never be reached -- added just to suppress compiler warning
}
