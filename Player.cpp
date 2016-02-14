#include <cmath>
#include <cstdlib>
#include <queue>
#include <vector>

#include "Player.h"
#include "constants.h"

Player::Player() {
    playerId = 1;
}

Player::Player(int playerId) {
    this->playerId = playerId;
}

int Player::getPlayerId() {
    return playerId;
}

int Player::otherPlayer() {
    if (playerId == PONE) {
        return PTWO;
    } else if (playerId == PTWO) {
        return PONE;
    }
    return playerId;
}

std::vector<Move> Player::getAvailableMoves(Board board) {
    int i, j, k, l;
    int dirX[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dirY[] = {-1, 0, 1, 1, 1, 0, -1, -1};
    std::vector<Move> availableMoves;

    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board.getValue(i, j) == playerId) {
                for (k=0 ; k<8 ; k++) {
                    int steps = 1;
                    while (i+steps*dirX[k] >= 0 && i+steps*dirX[k] < SIZE && j+steps*dirY[k] >= 0 && j+steps*dirY[k] < SIZE && board.getValue(i+steps*dirX[k], j+steps*dirY[k]) == EMPTY) {
                        int newPosX = i+steps*dirX[k];
                        int newPosY = j+steps*dirY[k];
                        board.setValue(i,j, EMPTY);
                        board.setValue(newPosX, newPosY, playerId);
                        for (l=0 ; l<8 ; l++) {
                            int arrowDist = 1;
                            while (newPosX+arrowDist*dirX[l] >= 0 && newPosX+arrowDist*dirX[l] < SIZE && newPosY+arrowDist*dirY[l] >= 0 && newPosY+arrowDist*dirY[l] < SIZE && board.getValue(newPosX+arrowDist*dirX[l], newPosY+arrowDist*dirY[l]) == EMPTY) {
                                availableMoves.push_back(Move(i, j, newPosX, newPosY, newPosX+arrowDist*dirX[l], newPosY+arrowDist*dirY[l]));
                                arrowDist++;
                            }
                        }
                        board.setValue(i,j,playerId);
                        board.setValue(newPosX, newPosY, EMPTY);
                        steps++;
                    }
                }
            }
        }
    }
    return availableMoves;
}

void Player::orderMoves(Board* board, std::vector<Move> & moves) {
    // return;
    int totalMoves = (int) moves.size();
    if (totalMoves == 0) {
        return;
    }
    int playerId = board->getValue(moves[0].srcX, moves[0].srcY);
    int evals[totalMoves];
    int i, j, key;
    Move move;

    for (i=0 ; i<totalMoves ; i++) {
        board->playMove(moves[i]);
//        evals[i] = eval(board, playerId);
        evals[i] = ai->history[moves[i].srcX][moves[i].srcY][moves[i].dstX][moves[i].dstY][moves[i].arrowX][moves[i].arrowY];
        board->undoMove(moves[i]);
    }

    for (i = 1; i < totalMoves; i++)
    {
        key = evals[i];
        move = moves[i];
        j = i-1;
        while (j >= 0 && evals[j] < key) // Use less than sign
        {
            evals[j+1] = evals[j];
            moves[j+1] = moves[j];
            j = j-1;
        }
        evals[j+1] = key;
        moves[j+1] = move;
    }
}

Move Player::getNextMove(Board board) {

    ai = new AI;
    int depth;
    int i;

    std::vector<Move> moves = getAvailableMoves(board);
    orderMoves(&board, moves);
    int movesSize = (int) moves.size();

    int currentScore, score ;
    int alpha , beta;
    int z = rand()%(int)moves.size();
    Move move = moves[z];
    Move nonFinalMove = move;

    if (playerId == PONE) {
        //printf("%d %d\n%d %d\n%d %d\n%d %d", move.srcX, move.srcY, move.dstX, move.dstY, move.arrowX, move.arrowY,depth, eval(&board, player_id));
        return move;
    }

    for (depth = 0; !ai->timeout() ; depth++) {
        score = -INF;
        alpha = -INF, beta = INF;
        // int n = beta;
        for (i = 0; i < movesSize && !ai->timeout(); i++) {
            board.playMove(moves[i]);
//            currentScore = negaMax(board, depth, otherPlayer(player_id));
            currentScore = ai->alphabeta(board, depth, otherPlayer(), -beta, -alpha);
            if (currentScore > score) {
                score = currentScore;
                ai->history[moves[i].srcX][moves[i].srcY][moves[i].dstX][moves[i].dstY][moves[i].arrowX][moves[i].arrowY] = score;
                nonFinalMove = moves[i];
            }
            if (currentScore > alpha) {
                alpha = currentScore;
            }
            board.undoMove(moves[i]);
            if (alpha >= beta) {
                break;
            }
        }
        if(!ai->timeExceeded) {
            move = nonFinalMove;
        }

    }


    // Initial hardcoded moves
    if (board.getTurnNumber() == 1) {
        move = Move(9,3,2,3,4,1);
    }
    if (board.getTurnNumber() == 2) {
        if (board.getValue(2,3) == PONE && board.getValue(4,1) == WALL) {
            move = Move(0,6,7,6,5,8);
        }
        if (board.getValue(2,6) == PONE && board.getValue(4,8) == WALL) {
            move = Move(0,3,7,3,5,1);
        }
    }
    
    delete(ai);
    return move;
}
