#ifndef BOTCHALLANGE_PLAYER_H
#define BOTCHALLANGE_PLAYER_H

#include "AI.h"
#include "Board.h"
#include <vector>

class Player {
    int playerId;
    AI *ai;

public:
    Player();
    Player(int playerId);

    int getPlayerId();
    int otherPlayer();

    std::vector<Move> getAvailableMoves(Board board);
    void orderMoves(Board* board, std::vector<Move> & moves);
    Move getNextMove(Board board);

};


#endif //BOTCHALLANGE_PLAYER_H
