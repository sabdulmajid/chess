#include "textobserver.h"
using namespace std;

TextObserver::TextObserver() {}

TextObserver::~TextObserver() {}

void TextObserver::notify(ChessBoard& board) {
    for (int i = 7; i >= 0; --i) {
      cout << i + 1 << " "; // index i starts at 0, so we must print our y coordinate at i+1
      for (int j = 0; j <= 7; j++) {
        Piece* p = board.getSquare(i, j);
        if (p == nullptr) { 
          if ((i + j) % 2 == 0) { // a dark square is marked in a tile when the sum of the x and y axes is EVEN
            cout << "_";
          } else { // a light square is marked in a tile when the sum of the x and y axes is ODD
            cout << " ";
          } 
        }
        else { 
          if(p->getIsWhite()) {
            cout << char(p->getPieceType() - 32); // print out the uppercase character if the piece is a white piece
          } else {
            cout << p->getPieceType(); // print out the lowercase character if the piece is a white piece
          }
        }
      }
      cout << endl;
    }

    // print out the final x coordinates, denoted as letters
    cout << endl << "  abcdefgh" << endl;
}
