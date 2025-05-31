#ifndef GRAPHICALOBSERVER_H
#define GRAPHICALOBSERVER_H
#include <vector>
#include <string> 
#include "window.h"
#include "observer.h"
#include "chessboard.h"

class ChessBoard;

class GraphicalObserver : public Observer {
    Xwindow& window; 
    std::vector<std::vector<char>> boardState;
    const int tileSize;
    
    public:
        GraphicalObserver(Xwindow& w, int tileWidth);

        void notify(ChessBoard& board) override;

        ~GraphicalObserver();
};
#endif
