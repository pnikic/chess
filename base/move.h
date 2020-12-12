#ifndef MOVE_H
#define MOVE_H

#include "base.h"
#include "square.h"

class Move
{
public:
    Move();

    Move(const Square &a, const Square &b, PieceType p = NONE);

    // Constructs Move from an UCI string (e.g. a7a8N or g1f3)
    Move(const std::string &s);

    Move(const Move &m);

    std::string UCI() const;

    Square From() const;

    Square To() const;

    PieceType Promotion() const;

    Move& operator=(const Move& m);

private:
    Square from;
    Square to;
    PieceType promotion;
};

#endif //MOVE_H
