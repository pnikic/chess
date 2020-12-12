#include "setsqr.h"
#include <iostream>

SetSquares::SetSquares()
{
    set = 0;
}

SetSquares::SetSquares(uint64_t board)
{
    set = board;
}

SetSquares::SetSquares(const SetSquares &ss)
{
    set = ss.set;
}

void SetSquares::Add(const Square &s)
{
    set |= ((uint64_t)1 << s.Id());
}

void SetSquares::Remove(const Square &s)
{
    set &= ~((uint64_t)1 << s.Id());
}

int SetSquares::Count()
{
    return __builtin_popcountll(set);
}

bool SetSquares::IsEmpty()
{
    return set == 0;
}

bool SetSquares::IsEmpty(const Square &s)
{
    return (set & ((uint64_t)1 << s.Id())) == 0;
}

std::ostream& operator<<(std::ostream &buf, const SetSquares &ss)
{
    for (int i = 7; i >= 0; --i)
    {
        for (int j = 0; j < 8; ++j)
        {
            if (ss.set & ((uint64_t)1 << (i * 8 + j)))
                buf << "1";
            else
                buf << ".";
        }

        buf << "\n";
    }

    return buf;
}
