#include "setsqr.h"

SetSquares::SetSquares()
{
    set = 0;
}

SetSquares::SetSquares(uint64_t board)
{
    set = board;
}

SetSquares::SetSquares(const SetSquares& ss)
{
    set = ss.set;
}

void SetSquares::Add(const Square& s)
{
    set |= ((uint64_t)1 << s.Id());
}

void SetSquares::Remove(const Square& s)
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

bool SetSquares::IsEmpty(const Square& s)
{
    return (set & ((uint64_t)1 << s.Id())) == 0;
}

std::ostream& operator<<(std::ostream& buf, const SetSquares& ss)
{
    BoardArray Board = EmptyBoard;

    for (int i = 0; i < 64; ++i)
        if (ss.set & ((uint64_t)1 << i))
            Board[i / 8][i % 8] = '1';

    for (int i = 7; i >= 0; --i)
        buf << Board[i] << '\n';
    return buf;        
}
