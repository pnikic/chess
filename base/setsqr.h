#ifndef SETSQR_H
#define SETSQR_H

#include "base.h"
#include "square.h"

class SetSquares
{
public:
    SetSquares();

    SetSquares(uint64_t board);

    SetSquares(const SetSquares &ss);

    void Add(const Square &s);

    void Remove(const Square &s);

    int Count();

    bool IsEmpty();

    bool IsEmpty(const Square &s);

    friend std::ostream& operator<<(std::ostream &buf, const SetSquares &ss);

private:
    uint64_t set;
};

#endif //SETSQR_H
