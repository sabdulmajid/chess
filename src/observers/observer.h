#ifndef OBSERVER_H
#define OBSERVER_H
#include <iostream>
#include <vector>

class ChessBoard;

class Observer {
    public:
        virtual void notify(ChessBoard& board) = 0; 
        virtual ~Observer() = default;
};

#endif
