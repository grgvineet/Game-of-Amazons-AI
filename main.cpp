#include <iostream>
#include <vector>

#include "Board.h"
#include "constants.h"
#include "Player.h"

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

    Player player1(1), player2(2);

    board.setInitialBoard();

    while(board.getAvailableMoves(playerId).size() > 0) {
        std::cout << board.getTurnNumber() << std::endl;

        move = player1.getNextMove(board);
        board.playMove(move);
        board.print();
        std::cin.get();

        std::cout << board.getTurnNumber() << std::endl;
        board.print();

        playerId = otherPlayer(playerId);
        if (board.getAvailableMoves(playerId).size() <= 0) break;

        move = player2.getNextMove(board);
        board.playMove(move);
        board.print();
        std::cin.get();

        playerId = otherPlayer(playerId);
    }

    std::cout << "Player " << otherPlayer(playerId) << " wins!";
    return 0;
}

