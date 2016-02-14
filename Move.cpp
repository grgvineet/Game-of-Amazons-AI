#include "Move.h"

#include <fstream>

Move::Move() {

}

Move::Move (int sX,int sY,int dX,int dY,int aX,int aY) {
    srcX = sX;
    srcY = sY;
    dstX = dX;
    dstY = dY;
    arrowX = aX;
    arrowY = aY;
}

void Move::readFromFile() {
    std::ifstream file;
    file.open("output.txt");
    file >> srcX >> srcY >> dstX >> dstY >> arrowX >> arrowY;
    file.close();
}
