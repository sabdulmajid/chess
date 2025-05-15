#include "graphicalobserver.h"
using namespace std;

const int margin = 40;
vector<string> xAxes = {"A", "B", "C", "D", "E", "F", "G", "H"};

GraphicalObserver::GraphicalObserver(Xwindow& w, int tileWidth): tileSize{600 / tileWidth}, window{w} {
    // draw a default state of the board, while keeping it in a state as a field for the object
    for (int i = 0; i < tileWidth; ++i) {
        window.drawString(i * tileSize + tileSize / 2 + margin, 600 + margin + margin / 2, xAxes[i]); // draw the x-axis coordinates (denoted by letters)
        window.drawString(margin / 2, i * tileSize + tileSize / 2 + margin, to_string(8 - i)); // draw the y-axis coordinates (denoted by numbers)
        vector<char> emptyRow;
        for (int j = 0; j < tileWidth; ++j) {;
            emptyRow.push_back(' ');
            window.fillRectangle(j * tileSize + margin, i * tileSize + margin, tileSize, tileSize, (i + j) % 2 == 1 ? window.Green : window.White);
        }
        
        boardState.push_back(emptyRow);
    }
}

void GraphicalObserver::notify(ChessBoard& board) {

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* p = board.getSquare(7-i, j);

            // if the piece exists, store it in a variable to avoid invalid reads
            char pPieceType = 'x';
            if (p != nullptr) { pPieceType = p->getIsWhite() ? p->getPieceType() - 32 : p->getPieceType(); }

            // if the piece hasn't changed locations, continue to the next iteration. otherwise, update the new state
            if ((p == nullptr && boardState.at(7-i).at(j) == ' ') || (p != nullptr && (pPieceType == boardState.at(7-i).at(j)))) { continue; }
            else { 
                if (p == nullptr) { boardState.at(7-i).at(j) = ' '; }
                else {
                    boardState.at(7-i).at(j) = p->getIsWhite() ? p->getPieceType() - 32 : p->getPieceType();
                }
            }

            // covers and removes any previous string in the tile
            window.fillRectangle(j * tileSize + margin, i * tileSize + margin, tileSize, tileSize, (i + j) % 2 == 1 ? window.Green : window.White);
            
            // if a piece exists, add the piece type onto the tile
            if (p != nullptr) {
                char pieceType = p->getIsWhite() ? p->getPieceType() - 32 : p->getPieceType();
                string pieceTypeStr {pieceType};
                window.drawString(j * tileSize + tileSize / 2 - tileSize / 10 + margin, i * tileSize + tileSize / 2 + margin, pieceTypeStr);
            }
        }
    }
}

GraphicalObserver::~GraphicalObserver() {}
