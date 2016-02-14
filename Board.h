#ifndef BOTCHALLANGE_BOARD_H
#define BOTCHALLANGE_BOARD_H

#include "Move.h"

#include "vector"

class Board {

    int board[10][10];

public:

    Board();
    void setInitialBoard();

    int getValue(int x, int y);
    void setValue(int x, int y,int value);

    void input();
    void print();
    void writeToFile(int playerId);

    bool validateMove(const Move& move);
    void playMove(const Move& move);
    void undoMove(const Move& move);

    int getTurnNumber();

    std::vector<Move> getAvailableMoves(int playerId);

};


#endif //BOTCHALLANGE_BOARD_H
