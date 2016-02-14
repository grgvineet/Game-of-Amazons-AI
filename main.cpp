#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

#define SIZE 10
#define EMPTY 0
#define PONE 1
#define PTWO 2
#define WALL -1

#define TIMELIMIT 0.95

#define INF 1000000

#define INPUT_FILE input.txt
#define OUTPUT_FILE output.txt;

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

    void readFromFile() {
        ifstream file;
        file.open("output.txt");
        file >> srcX >> srcY >> dstX >> dstY >> arrowX >> arrowY;
        file.close();
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

    void playMove(const Move& move) {
        int player = board[move.srcX][move.srcY];
        board[move.srcX][move.srcY] = EMPTY;
        board[move.dstX][move.dstY] = player;
        board[move.arrowX][move.arrowY] = WALL;
    }

    void writeToFile(int playerId) {
        int i,j;
        ofstream file;
        file.open("input.txt");
        for (i=0 ; i<SIZE ; i++) {
            for(j=0 ; j<SIZE ; j++) {
                file << board[i][j] << " ";
            }
            file << endl;
        }
        file << playerId << endl;
        file.close();
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

int getTurnNumber(Board& board) {
    int i, j;
    int count = 0;
    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board.getValue(i, j) == WALL) {
                count++;
            }
        }
    }
    return count+1;
}

int otherPlayer(int playerId) {
    if (playerId == PONE) {
        return PTWO;
    } else if (playerId == PTWO) {
        return PONE;
    }
    return playerId;
}

int main() {

    Board board;
    Move move;
    int playerId = 1;

    board.setInitialBoard();

    while(getAvailableMoves(board, playerId).size() > 0) {
        cout << getTurnNumber(board) << endl;
        board.writeToFile(playerId);
        system("./Player < input.txt > output.txt");

        move.readFromFile();
        board.playMove(move);

        cout << getTurnNumber(board) << endl;

        playerId = otherPlayer(playerId);
        if (getAvailableMoves(board, playerId).size() <= 0) break;

        board.writeToFile(playerId);
        system("./Player < input.txt > output.txt");

        move.readFromFile();
        board.playMove(move);

        playerId = otherPlayer(playerId);
    }

    cout << "Player " << otherPlayer(playerId) << " wins!";
    return 0;
}

