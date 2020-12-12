#include "square.h"
#include "setsqr.h"

#include <iostream>

Square::Square()
{
    id = NS;
}

Square::Square(int rank, int file)
{
    // Validity check
    if (!Legal(rank, file))
        id = NS;
    else
        id = ToSquare(rank, file);
}

Square::Square(BitBoard b)
{
    // Bitboard must have exactly one active bit!
    if (countOnes(b) != 1)
        id = NS;
    else
        id = static_cast<SquareType>(LSB(b));
}

Square::Square(SquareType s)
{
    ASSERT(s <= NS, "Invalid square type!");
    id = s;
}

Square::Square(const Square &s)
{
    id = s.id;
}

Square::Square(const std::string &s)
{
    assert(s.size() == 2);
    size_t file = FileNames.find(s[0]);
    size_t rank = RankNames.find(s[1]);
    ASSERT(file != std::string::npos && rank != std::string::npos, "Invalid file or rank!");
    id = ToSquare(rank, file);
}


int Square::File() const
{
    return id != NS ? id % 8 : -1;
}

int Square::Rank() const
{
    return id != NS ? id / 8 : -1;
}

SquareType Square::Id() const
{
    return id;
}

BitBoard Square::BB() const
{
    return BBSquares[id];
}

int Square::Distance(const Square &s) const
{
    ASSERT(id <= H8 && s.id <= H8, "Invalid squares!");
    int rankDist = abs(Rank() - s.Rank());
    int fileDist = abs(File() - s.File());
    return std::max(rankDist, fileDist);
}

int Square::Direction(const Square &s) const
{
    ASSERT(id <= H8 && s.id <= H8, "Invalid squares!");
    int d1 = s.File() - File(), d2 = s.Rank() - Rank();
    if (d1 == 0 && d2 > 0)
        return 2;
    if (d1 == 0 && d2 < 0)
        return 6;
    if (d2 == 0 && d1 > 0)
        return 4;
    if (d2 == 0 && d1 < 0)
        return 0;
    if (d1 > 0 && d2 > 0 && d1 == d2)
        return 3;
    if (d1 > 0 && d2 < 0 && d1 == -d2)
        return 5;
    if (d1 < 0 && d2 > 0 && d1 == -d2)
        return 1;
    if (d1 < 0 && d2 < 0 && d1 == d2)
        return 7;

    return -1;
}

std::string Square::Name() const
{
    if (id != NS)
        return std::string(1, File() + 'a') + std::string(1, Rank() + '1');
    else
        return "00";
}

bool Square::operator==(const Square &sq) const
{
    return id == sq.Id();
}

bool Square::operator!=(const Square &sq) const
{
    return id != sq.Id();
}

Square& Square::operator=(const Square &sq)
{
    // Self-assignment check
    if (this != &sq)
        id = sq.id;

    return *this;
}

std::ostream& operator<<(std::ostream &buf, const Square &sq)
{
    return buf << SetSquares(1ull << sq.Id());
}
