#include "game.h"
using namespace std;

#ifndef NO_GRAPHICS
Game::Game(Xwindow* window): scoreWhite{0}, scoreBlack{0}, textDisplay{make_unique<TextObserver>()},
                            graphicalDisplay{window != nullptr ? make_unique<GraphicalObserver>(*window, 8) : nullptr}, 
                            board{make_unique<ChessBoard>(textDisplay.get(), graphicalDisplay.get())},
                            isWhiteTurn{true}, setupMode{false} {
    cout << "Chess Engine initialized with " << (window ? "graphics" : "console mode") << endl;
}
#else
Game::Game(void* unused): scoreWhite{0}, scoreBlack{0}, textDisplay{make_unique<TextObserver>()},
                         board{make_unique<ChessBoard>(textDisplay.get(), nullptr)},
                         isWhiteTurn{true}, setupMode{false} {
    cout << "Chess Engine initialized in console mode" << endl;
}
#endif

Game::~Game() {}

void Game::setupNormalBoard() {
    board->removeAllPieces();
    isWhiteTurn = true;

    // places white pieces
    board->placePiece(0, 0, true, 'r');
    board->placePiece(0, 1, true, 'n');
    board->placePiece(0, 2, true, 'b');
    board->placePiece(0, 3, true, 'q');
    board->placePiece(0, 4, true, 'k');
    board->placePiece(0, 5, true, 'b');
    board->placePiece(0, 6, true, 'n');
    board->placePiece(0, 7, true, 'r');

    // places white and white pawns
    for (int i = 0; i < 8; i++) {
        board->placePiece(1, i, true, 'p');
        board->placePiece(6, i, false, 'p');
    }

    // places black pawns
    board->placePiece(7, 0, false, 'r');
    board->placePiece(7, 1, false, 'n');
    board->placePiece(7, 2, false, 'b');
    board->placePiece(7, 3, false, 'q');
    board->placePiece(7, 4, false, 'k');
    board->placePiece(7, 5, false, 'b');
    board->placePiece(7, 6, false, 'n');
    board->placePiece(7, 7, false, 'r');

}

bool Game::runTurn() {
    if (isWhiteTurn) {
        return pWhite->makeMove(*board);
    } else {
        return pBlack->makeMove(*board);
    }
}

void Game::startGame(bool whiteIsHuman, bool blackIsHuman, int whiteDifficulty, int blackDifficulty) {
    if(!setupMode) { isWhiteTurn = true; }

    if (whiteIsHuman) { pWhite = make_unique<Human>(true); } 
    else { pWhite = make_unique<Computer>(true, whiteDifficulty); }

    if (blackIsHuman) { pBlack = make_unique<Human>(false); } 
    else { pBlack = make_unique<Computer>(false, blackDifficulty); }

    if (!setupMode) { setupNormalBoard(); } 
    board->notifyObservers();

    while (in) {
        bool result = runTurn();
        board->notifyObservers();

        isWhiteTurn = !isWhiteTurn;
        string nextPlayer = isWhiteTurn ? "white" : "black";
        string curPlayer = isWhiteTurn ? "black" : "white";

        if (!result) {
            out << "Player " << curPlayer << " resigns and "<< nextPlayer << " wins!" << endl; 
            if(curPlayer == "white") { scoreBlack += 1; }
            else { scoreWhite += 1; }
            break;
        }

        if (board->checkCheckmate(isWhiteTurn)) {
            out << "Checkmate! " << curPlayer << " wins!" << endl;
            if (curPlayer == "white") { scoreWhite += 1; }
            else if (curPlayer == "black") { scoreBlack += 1; }
            break;
        }

        if (board->checkStalemate(isWhiteTurn)) {
            scoreWhite += 0.5;
            scoreBlack += 0.5;
            out << "Stalemate!" << endl; 
            break;
        }

        if (board->checkIfKingIsInCheck(isWhiteTurn)) {
            out << nextPlayer << " is in check." << endl;
        }
        
    }
}


void Game::setupBoard() {
    setupMode = true;
    board->removeAllPieces();
    
    string inputLine;
    while (getline(in, inputLine)) {
        istringstream iss{inputLine};
        string command;
        iss >> command;

        if (command == "+") {
            char type;
			char col;
			int row;
			iss >> type >> col >> row;
            
            row--; 
            char lowerCaseType = tolower(type);
            if (row >= 0 && row < 8 && col >= 'a' && col <= 'h' && (lowerCaseType == 'p' || lowerCaseType == 'r' || lowerCaseType == 'n' || lowerCaseType == 'b' || lowerCaseType == 'q' || lowerCaseType == 'k')) {
                if(lowerCaseType == 'k' || lowerCaseType == 'r') {
                    board->placePiece(row, int(col - 'a'), type != lowerCaseType, lowerCaseType, true);
                } else if(lowerCaseType == 'p') {
                    if(lowerCaseType != type) {
                        if(row == 1) {
                            board->placePiece(row, int(col - 'a'), type != lowerCaseType, lowerCaseType, false);
                        } else {
                            board->placePiece(row, int(col - 'a'), type != lowerCaseType, lowerCaseType, true);
                        }
                    } else {
                        if(row == 6) {
                            board->placePiece(row, int(col - 'a'), type != lowerCaseType, lowerCaseType, false);
                        } else {
                            board->placePiece(row, int(col - 'a'), type != lowerCaseType, lowerCaseType, true);
                        }
                    }
                } else {
                    board->placePiece(row, int(col - 'a'), type != lowerCaseType, lowerCaseType, false);
                }
                board->notifyObservers();
            } else {
                cerr << "Invalid command in Game::setupBoard (+)" << endl;
            }
        } else if (command == "-") {
            char col;
			int row;
			iss >> col >> row;
            row--;
            if (row >= 0 && row < 8 && col >= 'a' && col <= 'h') {
                board->removePiece(row, int(col - 'a'));
                board->notifyObservers();
            } else {
                cerr << "Invalid command in Game::setupBoard (-)" << endl;
            }
        } else if (command == "=") { // makes it `colour`'s turn to go next
            string colour;
            iss >> colour;

            if (colour == "white") {
                isWhiteTurn = true;
            } else if (colour == "black") {
                isWhiteTurn = false;
            } else {
                cerr << "Invalid command in Game::setupBoard (=)" << endl;
            }
        } else if (command == "done") { // leave setup mode
            int blackKingCount = board->getNumKings(false); 
            int whiteKingCount = board->getNumKings(true);
            
            // verify that the board contains exactly one white king and exactly one black king
            if (blackKingCount != 1 || whiteKingCount != 1) {
                cerr << "Error: Invalid board setup, wrong number of kings" << endl; 
                continue;
            }

            // verify that neither kings are in check
            if (board->checkIfKingIsInCheck(true) || board->checkIfKingIsInCheck(false)) {
                cerr << "Error: invalid board setup, both kings in check" << endl;
                continue;
            }

            // verify that no pawns are in the first or last row of the board
            if (board->checkNoPawnsInLastRank()) {
                cerr << "Error: invalid board setup, pawns are in the last rank." << endl;
                continue;
            }

            break;
        } else {
            cerr << "Invalid command in Game::setupBoard (\"done\")" << endl;
        }
    }
}


void Game::renderScore () const{
    out << "Final Score:" << endl;
	out << "White: " << scoreWhite << endl;
	out << "Black: " << scoreBlack << endl;
}


