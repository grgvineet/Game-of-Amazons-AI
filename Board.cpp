#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

#include "Board.h"
#include "constants.h"

Board::Board() {

}

void Board::setValue(int x, int y,int value) {
    board[x][y] = value;
}

int Board::getValue(int x, int y) {
    return board[x][y];
}

void Board::setInitialBoard() {
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

bool Board::validateMove(const Move& move) {
    int dx, dy;
    dx = std::abs(move.srcX - move.dstX);
    dy = std::abs(move.srcY - move.dstY);
    if (dx == dy || dx == 0 || dy == 0) {
        dx = std::abs(move.dstX - move.arrowX);
        dy = std::abs(move.dstY - move.arrowY);
        if (dx == dy || dx == 0 || dy == 0) {
            if ((board[move.srcX][move.srcY] == PONE || board[move.srcX][move.srcY] == PTWO) && board[move.dstX][move.dstY] == EMPTY && board[move.arrowX][move.arrowY] == EMPTY) {
                return true;
            }
        }
    }
    return false;
}

void Board::playMove(const Move& move) {
    int player = board[move.srcX][move.srcY];
    board[move.srcX][move.srcY] = EMPTY;
    board[move.dstX][move.dstY] = player;
    board[move.arrowX][move.arrowY] = WALL;
}

void Board::undoMove(const Move& move) {
    int player = board[move.dstX][move.dstY];
    board[move.arrowX][move.arrowY] = EMPTY;
    board[move.dstX][move.dstY] = EMPTY;
    board[move.srcX][move.srcY] = player;
}

int Board::getTurnNumber() {
    int i, j;
    int count = 0;
    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (board[i][j] == WALL) {
                count++;
            }
        }
    }
    return count+1;
}

std::vector<Move> Board::getAvailableMoves(int playerId) {
    int i, j, k, l;
    int dirX[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int dirY[] = {-1, 0, 1, 1, 1, 0, -1, -1};
    std::vector<Move> availableMoves;

    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            if (getValue(i, j) == playerId) {
                for (k=0 ; k<8 ; k++) {
                    int steps = 1;
                    while (i+steps*dirX[k] >= 0 && i+steps*dirX[k] < SIZE && j+steps*dirY[k] >= 0 && j+steps*dirY[k] < SIZE && getValue(i+steps*dirX[k], j+steps*dirY[k]) == EMPTY) {
                        int newPosX = i+steps*dirX[k];
                        int newPosY = j+steps*dirY[k];
                        setValue(i,j, EMPTY);
                        setValue(newPosX, newPosY, playerId);
                        for (l=0 ; l<8 ; l++) {
                            int arrowDist = 1;
                            while (newPosX+arrowDist*dirX[l] >= 0 && newPosX+arrowDist*dirX[l] < SIZE && newPosY+arrowDist*dirY[l] >= 0 && newPosY+arrowDist*dirY[l] < SIZE && getValue(newPosX+arrowDist*dirX[l], newPosY+arrowDist*dirY[l]) == EMPTY) {
                                availableMoves.push_back(Move(i, j, newPosX, newPosY, newPosX+arrowDist*dirX[l], newPosY+arrowDist*dirY[l]));
                                arrowDist++;
                            }
                        }
                        setValue(i,j,playerId);
                        setValue(newPosX, newPosY, EMPTY);
                        steps++;
                    }
                }
            }
        }
    }
    return availableMoves;
}

void Board::writeToFile(int playerId) {
    int i,j;
    std::ofstream file;
    file.open("input.txt");
    for (i=0 ; i<SIZE ; i++) {
        for(j=0 ; j<SIZE ; j++) {
            file << board[i][j] << " ";
        }
        file << std::endl;
    }
    file << playerId << std::endl;
    file.close();
}

void Board::input() {
    int i, j;
    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            std::cin >> board[i][j];
        }
    }
}

void Board::print() {
    std::system("clear");
    int i,j;
    for (i=0 ; i<SIZE ; i++) {
        for (j=0 ; j<SIZE ; j++) {
            std::cout << std::setw(5) << board[i][j];
        }
        std::cout << std::endl;
    }
}
