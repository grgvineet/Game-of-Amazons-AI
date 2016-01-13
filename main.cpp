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

#define TIMELIMIT 0.9

#define INF 100000000

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
void orderMoves(Board* board, vector<Move> & moves);

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
int negaScout(Board board, int depth, int playerId ,int alpha, int beta);

clock_t start = clock();
bool timeExceeded = false;
bool timeout();

class Move {

public:
    int srcX, srcY, dstX, dstY, arrowX, arrowY;

    Move() {

    }

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

    void setValue(int x, int y,int value) {
        board[x][y] = value;
    }

    int getValue(int x, int y) {
        return board[x][y];
    }

    void setInitialBoard() {
        // Just for testing purpose
        // Or clearing the board
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
                if (availableMove != 0 && availableMove < minOne) { // Assuming that this condition will be true for at least one queen, since if availableMove is zero for all, game ends
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
                if (availableMove != 0 && availableMove < minTwo) {
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
    int arr[SIZE][SIZE] = {0};
    int i, j, k;

    int dirX[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dirY[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    vector<pair<int, int> > pOnePieces, pTwoPieces;

    int region = 1;
    int count;
    vector<int> regionCount;
    regionCount.push_back(0);

    // Dividing board into regions and marking regions in a separate array
    for (i=0 ; i<SIZE ; i++) {
        for (j=0; j<SIZE ; j++) {
            if (board->getValue(i, j) == EMPTY && arr[i][j] == 0) {
                arr[i][j] = region;
                count = 1;
                queue<pair<int, int> > q;
                q.push(make_pair(i,j));
                while (!q.empty()) {
                    pair<int, int> front = q.front();
                    q.pop();
                    int x = front.first, y = front.second;
                    for (k=0 ; k<8 ; k++) {
                        if (x+dirX[k]>=0 && x+dirX[k]<SIZE && y+dirY[k]>=0 &&
                            y+dirY[k]<SIZE && board->getValue(x+dirX[k], y+dirY[k]) == EMPTY && arr[x+dirX[k]][y+dirY[k]] == 0) {
                            arr[x+dirX[k]][y+dirY[k]] = region;
                            count++;
                            q.push(make_pair(x+dirX[k], y+dirY[k]));
                        }

                    }
                }
                regionCount.push_back(count);
                region++;
            } else if (board->getValue(i, j) == PONE) {
                pOnePieces.push_back(make_pair(i,j));
            } else if (board->getValue(i, j) == PTWO) {
                pTwoPieces.push_back(make_pair(i, j));
            }
        }
    }

    // Two dimensional table in which value is true if piece in that region
    bool pOneRegionInfo[100][4] = {0}, pTwoRegionInfo[100][4] = {0};
    for (i=0 ; i<4 ; i++) {
        int x = pOnePieces[i].first, y = pOnePieces[i].second;
        for (k=0 ; k<8 ; k++) {
            if (x+dirX[k]>=0 && x+dirX[k]<SIZE && y+dirY[k]>=0 &&
                y+dirY[k]<SIZE && arr[x+dirX[k]][y+dirY[k]] != 0) {
                pOneRegionInfo[arr[x+dirX[k]][y+dirY[k]]][i] = true;
            }
        }

        x = pTwoPieces[i].first;
        y = pTwoPieces[i].second;
        for (k=0 ; k<8 ; k++) {
            if (x+dirX[k]>=0 && x+dirX[k]<SIZE && y+dirY[k]>=0 &&
                y+dirY[k]<SIZE && arr[x+dirX[k]][y+dirY[k]] != 0) {
                pTwoRegionInfo[arr[x+dirX[k]][y+dirY[k]]][i] = true;
            }
        }
    }

    // This calculates piece value for all pieces
    // By measuring dominant regions and shared regions of a piece on board
    int pOneValue[4], pTwoValue[4];
    for (i=0 ; i<4 ; i++) {
        int numDominant = 0, numShared = 0;
        for (j=1; j<region ; j++) {
            if (pOneRegionInfo[j][i]) {
                bool flag = false;
                for (k=0 ; k<4 ; k++) {
                    if (pTwoRegionInfo[j][k]) {
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    numDominant++;
                } else {
                    numShared++;
                }
            }
        }
        pOneValue[i] = 2*numDominant + numShared;
    }

    for (i=0 ; i<4 ; i++) {
        int numDominant = 0, numShared = 0;
        for (j=1; j<region ; j++) {
            if (pTwoRegionInfo[j][i]) {
                bool flag = false;
                for (k=0 ; k<4 ; k++) {
                    if (pOneRegionInfo[j][k]) {
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    numDominant++;
                } else {
                    numShared++;
                }
            }
        }
        pTwoValue[i] = 2*numDominant + numShared;
    }

    // Calculating score for each player
    float pOneScore = 0.0;
    float pTwoScore = 0.0;
    for (i=1 ; i<region ; i++) {
        int score = regionCount[i];
        float pOneNum = 0.0, pTwoNum = 0.0;

        for (j=0 ; j<4 ; j++) {
            if (pOneRegionInfo[i][j]) {
                pOneNum += 1.0/pOneValue[j];
            }
            if (pTwoRegionInfo[i][j]) {
                pTwoNum += 1.0/pTwoValue[j];
            }
        }
        if (pOneNum == 0.0 && pTwoNum == 0.0) {
            // For closed regions which do not have either black or white
            // This caused zero by zero exception
            continue;
        }
        pOneScore += score*pOneNum/(pOneNum + pTwoNum);
        pTwoScore += score*pTwoNum/(pOneNum + pTwoNum);
    }

    // Calculating final return value
    int value = (int)((1000*(pOneScore - pTwoScore))/(pOneScore + pTwoScore));
    if (playerId == PONE) {
        return value;
    } else {
        return -value;
    }

}

int eval(Board* board, int playerId) {
    //return mobility(board, playerId);
    //return territory(board, playerId);
    //return territoryAndMobility(board, playerId);

    // Best eval
    // 2*regions + 5*territory + 3*minmobility
//    int region = regions(board, playerId);
//    int territor = territory(board, playerId);
//    int mobile = minMobility(board, playerId);
//    printf("%d %d %d\n", region, territor, mobile);
//    return 2*region + 5*territor + 3*mobile;
    return 2*regions(board, playerId) + 5*territory(board, playerId) + 3*minMobility(board, playerId);
    //return regions(board, playerId);
}

void orderMoves(Board* board, vector<Move> & moves) {
    return;
    int totalMoves = (int) moves.size();
    if (totalMoves == 0) {
        return;
    }
    int playerId = board->getValue(moves[0].srcX, moves[0].srcY);
    int evals[totalMoves];
    int i, j, key;
    Move move;

    for (i=0 ; i<totalMoves ; i++) {
        playMove(board, &moves[i]);
        evals[i] = eval(board, playerId);
        undoMove(board, &moves[i]);
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

    if (depth == 0 || timeout()) {
        return eval(&board, otherPlayer(playerId));
    }

    int score = -INF, currentScore;
    int i;
    int movesSize;
    vector<Move> moves = getAvailableMoves(board, playerId);
    movesSize = (int)moves.size();
    for (i=0 ; i<movesSize ; i++) {
        playMove(&board, &moves[i]);
        currentScore = negaMax(board, depth-1, otherPlayer(playerId));
        if (currentScore > score) {
            score = currentScore;
        }
        undoMove(&board, &moves[i]);
        if (timeout()) {
            break;
        }
    }
    return -score;
}

int alphabeta(Board board, int depth, int playerId, int alpha, int beta) {

    if (depth == 0 || timeout()) {
        return eval(&board, otherPlayer(playerId));
    }

    int score = -INF, currentScore;
    vector<Move> moves = getAvailableMoves(board, playerId);
    orderMoves(&board, moves);
    int movesSize = (int) moves.size();
    int i=0;

    for (i=0 ; i<movesSize ; i++) {
        playMove(&board, &moves[i]);
        currentScore = alphabeta(board, depth - 1, otherPlayer(playerId),  -beta, -alpha);
        if (currentScore > score) {
            score = currentScore;
        }
        if (currentScore > alpha) {
            alpha = currentScore;
        }
        undoMove(&board, &moves[i]);
        if (alpha >= beta || timeout()) {
            break;
        }
    }
    return -score;
}

int negaScout(Board board, int depth, int playerId, int alpha, int beta) {
    if (depth == 0 || timeout()) {
        return eval(&board, playerId);
    }

    int score = -INF, currentScore;
    int n = beta;

    vector<Move> moves = getAvailableMoves(board, playerId);
    orderMoves(&board, moves);
    int movesSize = (int) moves.size();
    int i=0;

    for (i=0 ; i<movesSize ; i++) {
        playMove(&board, &moves[i]);
        currentScore = negaScout(board, depth - 1, otherPlayer(playerId),  -n, -alpha);
        if (currentScore > score) {
            if (n == beta || depth <= 1) {
                score = currentScore;
            } else {
                score = negaScout(board, depth - 1, otherPlayer(playerId), -beta, -currentScore);
            }
        }
        if (currentScore > alpha) {
            alpha = currentScore;
        }
        undoMove(&board, &moves[i]);
        if (alpha >= beta || timeout()) {
            break;
        }
        n = alpha + 1;
    }
    return -score;

}

bool timeout() {
    if (((float) ((clock() - start)) / CLOCKS_PER_SEC) > TIMELIMIT) {
        timeExceeded = true;
        return true;
    }
    return false;
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
    orderMoves(&board, moves);
    int movesSize = (int) moves.size();

    int currentScore, score ;
    int alpha , beta;
    int z = rand()%(int)moves.size();
    Move move = moves[z];
    Move nonFinalMove = move;

    for (depth = 0; !timeout() ; depth++) {
        score = -INF;
        alpha = -INF, beta = INF;
        // int n = beta;
        for (i = 0; i < movesSize && !timeout(); i++) {
            playMove(&board, &moves[i]);
//            currentScore = negaMax(board, depth, otherPlayer(player_id));
            currentScore = alphabeta(board, depth, otherPlayer(player_id), -beta, -alpha);
            if (currentScore > score) {
                score = currentScore;
                nonFinalMove = moves[i];
            }
            if (currentScore > alpha) {
                alpha = currentScore;
            }
            undoMove(&board, &moves[i]);
            if (alpha >= beta) {
                break;
            }
        }
        if(!timeExceeded) {
            move = nonFinalMove;
        }

    }
//    int z = rand()%(int)moves.size();
//    printf("%d %d\n%d %d\n%d %d",moves[z].srcX, moves[z].srcY, moves[z].dstX, moves[z].dstY, moves[z].arrowX, moves[z].arrowY );
    printf("%d %d\n%d %d\n%d %d\n%d", move.srcX, move.srcY, move.dstX, move.dstY, move.arrowX, move.arrowY,depth);

    return 0;
}
