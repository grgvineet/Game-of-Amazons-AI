#ifndef BOTCHALLANGE_MOVE_H
#define BOTCHALLANGE_MOVE_H

class Move {

public:
    int srcX, srcY, dstX, dstY, arrowX, arrowY;

    Move();
    Move (int sX,int sY,int dX,int dY,int aX,int aY);
    void readFromFile();
};


#endif //BOTCHALLANGE_MOVE_H
