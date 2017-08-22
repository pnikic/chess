#ifndef MOVE_H
#define MOVE_H

#include "base.h"
#include "square.h"

class Move
{
public:
    Move();
    Move(Square& a, Square& b, PieceType p = NONE);
    Move(std::string s);
    Move(const Move& m);
    ~Move();
    std::string GetUCI() const;

private:
    Square from;
    Square to;
    PieceType promotion;    
};

#endif //MOVE_H
