#ifndef TEXTOBSERVER_H
#define TEXTOBSERVER_H
#include <vector>
#include <string> 
#include "observer.h"
#include "chessboard.h"
#include "piece.h"

class TextObserver : public Observer {

    public:
        TextObserver();
        ~TextObserver();

        void notify(ChessBoard& board) override;
};
#endif
