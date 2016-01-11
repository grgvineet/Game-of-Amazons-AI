#include <cstdio>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <cmath>
using namespace std;

#define SIZE 10
#define EMPTY 0
#define PONE 1
#define PTWO 2
#define WALL -1

#define INF 10000

class Board;
class Move;

vector<Move> getAvailableMoves(Board board, int playerId);

// Heuristics Functions
int mobility(Board* board, int playerId);
int territory(Board* board, int playerId);
int territoryAndMobility(Board* board, int playerId);
int minMobility(Board* board, int playerId);
int regions(Board* board, int playerId);
int eval(Board* board, int playerId);

// Move playing functions
void playMove(Board* board, Move* move);
void undoMove(Board* board, Move* move);

// General game play functions
int getTurnNumber(Board* board);
int otherPlayer(int playerId);
bool validateMove(Move move);

// Input function
void input(Board* board, int* player_id);

// AI functions
int negaMax(Board board, int depth, int playerId);
int alphabeta(Board board, int depth, int playerId, int alpha, int beta);

class Move {

public:
    int srcX, srcY, dstX, dstY, arrowX, arrowY;

    Move (int sX,int sY,int dX,int dY,int aX,int aY) {
        srcX = sX;
        srcY = sY;
        dstX = dX;
        dstY = dY;
        arrowX = aX;
        arrowY = aY;
    }
};

class Board {

    int board[10][10];

public:

    Board() {
//         setInitialBoard();
    }

    void setValue(int x, int y,int value) {
        board[x][y] = value;
    }

    int getValue(int x, int y) {
        return board[x][y];
    }

    void setInitialBoard() {
        int i, j;

        for (i=0 ; i<SIZE ; i++) {
            for (j=0 ; j<SIZE ; j++) {
                setValue(i, j, 0);
            }
        }
        setValue(0, 3, 2);
        setValue(0, 6, 2);
        setValue(3, 0, 2);
        setValue(3, 9, 2);
        setValue(6, 0, 1);
        setValue(6, 9, 1);
        setValue(9, 3, 1);
        setValue(9, 6, 1);
    }
};

vector<Move> getAvailableMoves(Board board, int playerId) {
    int i, j, k, l;
    int dirX[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dirY[] = {-1, 0, 1, 1, 1, 0, -1, -1};
    vector<Move> availableMoves;

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

int mobility(Board* board, int playerId) {
    int countPlayer = 0, countOtherPlayer = 0;
    countPlayer = (int)getAvailableMoves(*board, playerId).size();
    countOtherPlayer = (int)getAvailableMoves(*board, otherPlayer(playerId)).size();
    return countPlayer - countOtherPlayer;
}

int territory(Board* board, int playerId) {
    int i,j,k;
    int x, y;
    int pOneSquares = 0, pTwoSquares = 0;
    int pOneStonePly[SIZE][SIZE], pTwoStonePly[SIZE][SIZE];

    int dirX[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dirY[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for(i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            pOneStonePly[i][j] = INF;
            pTwoStonePly[i][j] = INF;
        }
    }

    for(i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board->getValue(i,j) == PONE) {
                pOneStonePly[i][j] = 0;
                queue<pair<int, int> > q;
                q.push(make_pair(i,j));
                while (!q.empty()) {
                    pair<int, int> front = q.front();
                    int a = front.first, b = front.second;
                    q.pop();
                    for (k=0 ; k<8 ; k++) {
                        int steps = 1;
                        while (a+steps*dirX[k] >= 0 && a+steps*dirX[k] < SIZE && b+steps*dirY[k] >= 0 && b+steps*dirY[k] < SIZE && board->getValue(a+steps*dirX[k], b+steps*dirY[k]) == EMPTY) {
                            if (pOneStonePly[a][b] < pOneStonePly[a+steps*dirX[k]][b+steps*dirY[k]]) {
                                pOneStonePly[a+steps*dirX[k]][b+steps*dirY[k]] = pOneStonePly[a][b] + 1;
                                q.push(make_pair(a+steps*dirX[k], b+steps*dirY[k]));
                            } else {
                                break;
                            }
                            steps++;
                        }
                    }
                }

            } else if (board->getValue(i, j) == PTWO) {
                pTwoStonePly[i][j] = 0;
                queue<pair<int, int> > q;
                q.push(make_pair(i,j));
                while (!q.empty()) {
                    pair<int, int> front = q.front();
                    int a = front.first, b = front.second;
                    q.pop();
                    for (k=0 ; k<8 ; k++) {
                        int steps = 1;
                        while (a+steps*dirX[k] >= 0 && a+steps*dirX[k] < SIZE && b+steps*dirY[k] >= 0 && b+steps*dirY[k] < SIZE && board->getValue(a+steps*dirX[k], b+steps*dirY[k]) == EMPTY) {
                            if (pTwoStonePly[a][b] < pTwoStonePly[a+steps*dirX[k]][b+steps*dirY[k]]) {
                                pTwoStonePly[a+steps*dirX[k]][b+steps*dirY[k]] = pTwoStonePly[a][b] + 1;
                                q.push(make_pair(a+steps*dirX[k], b+steps*dirY[k]));
                            } else {
                                break;
                            }
                            steps++;
                        }
                    }
                }
            }
        }
    }


    for(i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board->getValue(i,j) == EMPTY) {
                if (pOneStonePly[i][j] < pTwoStonePly[i][j]) {
                    pOneSquares++;
                } else if (pTwoStonePly[i][j] < pOneStonePly[i][j]) {
                    pTwoSquares++;
                }
            }
        }
    }

    if (playerId == PONE) {
        return pOneSquares - pTwoSquares;
    } else {
        return pTwoSquares - pOneSquares;
    }

}

int territoryAndMobility(Board* board, int playerId) {
    int i,j,k;
    int weight = 4;
    int pOnePoints = 0, pTwoPoints = 0;
    int pOneStonePly[SIZE][SIZE], pTwoStonePly[SIZE][SIZE];
    int pOneMob[SIZE][SIZE], pTwoMob[SIZE][SIZE];

    int dirX[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dirY[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            pOneStonePly[i][j] = INF;
            pTwoStonePly[i][j] = INF;
            pOneMob[i][j] = 0;
            pTwoMob[i][j] = 0;
        }
    }

    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board->getValue(i, j) == PONE) {

                pOneStonePly[i][j] = 0;
                queue<pair<int, int> > q;
                q.push(make_pair(i,j));
                while (!q.empty()) {
                    pair<int, int> front = q.front();
                    int a = front.first, b = front.second;
                    q.pop();
                    for (k=0 ; k<8 ; k++) {
                        int steps = 1;
                        while (a+steps*dirX[k] >= 0 && a+steps*dirX[k] < SIZE && b+steps*dirY[k] >= 0 && b+steps*dirY[k] < SIZE && board->getValue(a+steps*dirX[k], b+steps*dirY[k]) == EMPTY) {
                            if (pOneStonePly[a][b] < pOneStonePly[a+steps*dirX[k]][b+steps*dirY[k]]) {
                                pOneStonePly[a+steps*dirX[k]][b+steps*dirY[k]] = pOneStonePly[a][b] + 1;
                                q.push(make_pair(a+steps*dirX[k], b+steps*dirY[k]));
                            } else {
                                break;
                            }
                            steps++;
                        }
                    }
                }

                for (k=0 ; k<8 ; k++) {
                    int steps = 1;
                    while (i+steps*dirX[k] >= 0 && i+steps*dirX[k] < SIZE && j+steps*dirY[k] >= 0 && j+steps*dirY[k] < SIZE && board->getValue(i+steps*dirX[k], j+steps*dirY[k]) == EMPTY) {
                        pOneMob[i+steps*dirX[k]][j+steps*dirY[k]]++;
                        steps++;
                    }
                }
            } else if (board->getValue(i, j) == PTWO) {

                pTwoStonePly[i][j] = 0;
                queue<pair<int, int> > q;
                q.push(make_pair(i,j));
                while (!q.empty()) {
                    pair<int, int> front = q.front();
                    int a = front.first, b = front.second;
                    q.pop();
                    for (k=0 ; k<8 ; k++) {
                        int steps = 1;
                        while (a+steps*dirX[k] >= 0 && a+steps*dirX[k] < SIZE && b+steps*dirY[k] >= 0 && b+steps*dirY[k] < SIZE && board->getValue(a+steps*dirX[k], b+steps*dirY[k]) == EMPTY) {
                            if (pTwoStonePly[a][b] < pTwoStonePly[a+steps*dirX[k]][b+steps*dirY[k]]) {
                                pTwoStonePly[a+steps*dirX[k]][b+steps*dirY[k]] = pTwoStonePly[a][b] + 1;
                                q.push(make_pair(a+steps*dirX[k], b+steps*dirY[k]));
                            } else {
                                break;
                            }
                            steps++;
                        }
                    }
                }

                for (k=0 ; k<8 ; k++) {
                    int steps = 1;
                    while (i+steps*dirX[k] >= 0 && i+steps*dirX[k] < SIZE && j+steps*dirY[k] >= 0 && j+steps*dirY[k] < SIZE && board->getValue(i+steps*dirX[k], j+steps*dirY[k]) == EMPTY) {
                        pTwoMob[i+steps*dirX[k]][j+steps*dirY[k]]++;
                        steps++;
                    }
                }
            }
        }
    }

    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board->getValue(i, j) == EMPTY) {
                if (pOneStonePly[i][j] < pTwoStonePly[i][j]) {
                    pOnePoints += weight * pOneMob[i][j];
                } else if (pTwoStonePly[i][j] < pOneStonePly[i][j]) {
                    pTwoPoints += weight * pTwoMob[i][j];
                }
            }
        }
    }

    if (playerId == PONE) {
        return pOnePoints - pTwoPoints;
    } else {
        return pTwoPoints - pOnePoints;
    }

}

int minMobility(Board* board, int playerId) {
    int i,j,k;
    int minOne = INF, minTwo = INF;

    int dirX[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dirY[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (i=0 ; i<SIZE ; i++) {
        for (j = 0; j < SIZE; j++) {
            if (board->getValue(i, j) == PONE) {
                int availableMove = 0;
                for (k=0 ; k<8 ; k++) {
                    int steps = 1;
                    while (i+steps*dirX[k] >= 0 && i+steps*dirX[k] < SIZE && j+steps*dirY[k] >= 0 && j+steps*dirY[k] < SIZE && board->getValue(i+steps*dirX[k], j+steps*dirY[k]) == EMPTY) {
                        availableMove++;
                        steps++;
                    }
                }
                if (availableMove < minOne) {
                    minOne = availableMove;
                }
            } else if (board->getValue(i, j) == PTWO) {
                int availableMove = 0;
                for (k=0 ; k<8 ; k++) {
                    int steps = 1;
                    while (i+steps*dirX[k] >= 0 && i+steps*dirX[k] < SIZE && j+steps*dirY[k] >= 0 && j+steps*dirY[k] < SIZE && board->getValue(i+steps*dirX[k], j+steps*dirY[k]) == EMPTY) {
                        availableMove++;
                        steps++;
                    }
                }
                if (availableMove < minTwo) {
                    minTwo = availableMove;
                }
            }
        }
    }

    if (playerId == PONE) {
        return minOne - minTwo;
    } else {
        return minTwo - minOne;
    }
}

int regions(Board* board, int playerId) {
    return 0;
}

int eval(Board* board, int playerId) {
    //return mobility(board, playerId);
    //return territory(board, playerId);
    //return territoryAndMobility(board, playerId);

    // Best eval
    // 2*regions + 5*territory + 3*minmobility
    return 2*regions(board, playerId) + 5*territory(board, playerId) + 3*minMobility(board, playerId);
}

void playMove(Board* board, Move* move) {
    int playerId = board->getValue(move->srcX, move->srcY);
    board->setValue(move->srcX, move->srcY, EMPTY);
    board->setValue(move->dstX, move->dstY, playerId);
    board->setValue(move->arrowX, move->arrowY, WALL);
}

void undoMove(Board* board, Move* move) {
    int playerId = board->getValue(move->dstX, move->dstY);
    board->setValue(move->arrowX, move->arrowY, EMPTY);
    board->setValue(move->dstX, move->dstY, EMPTY);
    board->setValue(move->srcX, move->srcY, playerId);
}

int getTurnNumber(Board* board) {
    int i, j;
    int count = 0;
    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board->getValue(i, j) == WALL) {
                count++;
            }
        }
    }
    return count/2 + 1;
}

bool validateMove(Move move) {
    int dx, dy;
    dx = abs(move.srcX - move.dstX);
    dy = abs(move.srcY - move.dstY);
    if (dx == dy || dx == 0 || dy == 0) {
        dx = abs(move.dstX - move.arrowX);
        dy = abs(move.dstY - move.arrowY);
        if (dx == dy || dx == 0 || dy == 0) {
            return true;
        }
    }
    return false;
}

int otherPlayer(int playerId) {
    if (playerId == PONE) {
        return PTWO;
    } else if (playerId == PTWO) {
        return PONE;
    }
    return playerId;
}

void input(Board* board, int* player_id) {
    int i, j, t;

    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            scanf ("%d", &t);
            board->setValue(i, j, t);
        }
    }
    scanf ("%d", player_id);
}

int negaMax(Board board, int depth, int playerId) {

    if (depth == 0) {
        return eval(&board, otherPlayer(playerId));
    }

    int score = -INF, currentScore;
    int i;
    int movesSize;
    vector<Move> moves = getAvailableMoves(board, playerId);
    movesSize = (int)moves.size();
    for (i=0 ; i<movesSize ; i++) {
        playMove(&board, &moves[i]);
        currentScore = -negaMax(board, depth-1, otherPlayer(playerId));
        if (currentScore > score) {
            score = currentScore;
        }
        undoMove(&board, &moves[i]);
    }
    return score;
}

int alphabeta(Board board, int depth, int playerId, int alpha, int beta) {

    if (depth == 0) {
        return eval(&board, otherPlayer(playerId));
    }

    int score = -INF, currentScore;
    vector<Move> moves = getAvailableMoves(board, playerId);
    int movesSize = (int) moves.size();
    int i=0;

    for (i=0 ; i<movesSize ; i++) {
        playMove(&board, &moves[i]);
        currentScore = -alphabeta(board, depth - 1, otherPlayer(playerId),  -beta, -alpha);
        if (currentScore > score) {
            score = currentScore;
        }
        if (currentScore > alpha) {
            alpha = score;
        }
        undoMove(&board, &moves[i]);
        if (alpha > beta) {
            break;
        }
    }
    return score;
}

int main() {
    int i,j;
    int t;
    int depth;
    int player_id;
    Board board;

    srand(time(NULL));
//    player_id = 1;
//    board.setInitialBoard();
    input(&board, &player_id);

    vector<Move> moves = getAvailableMoves(board, player_id);
    int movesSize = (int) moves.size();


    int turn = getTurnNumber(&board);
    if (turn > 50) depth = 4;
    else if (turn > 40) depth = 3;
    else if (turn > 30) depth = 2;
    else if (turn > 20) depth = 1;
    else depth = 0;
//    depth = 0;

//    printf("Turn = %d; Depth = %d\n", turn, depth);

    int currentScore, score = -INF;
//    int alpha = -INF, beta = INF;
    int z = rand()%(int)moves.size();
    Move move = moves[z];
    for (i=0 ; i<movesSize ; i++) {
        playMove(&board, &moves[i]);
//        currentScore = alphabeta(board, depth, otherPlayer(player_id), alpha, beta);
        currentScore = negaMax(board, depth, otherPlayer(player_id));
        if (currentScore > score) {
            score = currentScore;
            move = moves[i];
        }
//        if (currentScore > alpha) {
//            alpha = currentScore;
//        }
        undoMove(&board, &moves[i]);
//        if (alpha >= beta) {
//            break;
//        }

    }
//    int z = rand()%(int)moves.size();
//    printf("%d %d\n%d %d\n%d %d",moves[z].srcX, moves[z].srcY, moves[z].dstX, moves[z].dstY, moves[z].arrowX, moves[z].arrowY );
    printf("%d %d\n%d %d\n%d %d\n", move.srcX, move.srcY, move.dstX, move.dstY, move.arrowX, move.arrowY);

    return 0;
}
