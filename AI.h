#ifndef BOTCHALLANGE_AI_H
#define BOTCHALLANGE_AI_H

#include "ctime"
#include "vector"

#include "Board.h"
#include "constants.h"

class AI {

    clock_t start;

    int otherPlayer(int playerId);

    // Heuristics Functions
    int mobility(Board* board, int playerId);
    int territory(Board* board, int playerId);
    int territoryAndMobility(Board* board, int playerId);
    int minMobility(Board* board, int playerId);
    int regions(Board* board, int playerId);
    int eval(Board* board, int playerId);
    void orderMoves(Board* board, std::vector<Move> & moves);


public:

    int history[SIZE][SIZE][SIZE][SIZE][SIZE][SIZE];
    bool timeExceeded;

    AI();

    bool timeout();
    // AI functions
    int negaMax(Board board, int depth, int playerId);
    int alphabeta(Board board, int depth, int playerId, int alpha, int beta);
    int negaScout(Board board, int depth, int playerId ,int alpha, int beta);
};


#endif //BOTCHALLANGE_AI_H
