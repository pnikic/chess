#include "square.h"

Square::Square()
{
    id = NS;
}

Square::Square(int file, int rank)
{
    assert(file >= 0 && rank >= 0 && file < 8 && rank < 8);
    id = rank * 8 + file;
}

Square::Square(SquareType s)
{
    assert(s <= H8);
    id = s;
}

Square::Square(Square& s)
{
    id = s.id;
}

Square::~Square(){}

FileType Square::GetFile() const
{
    return id % 8;
}

RankType Square::GetRank() const
{
    return id / 8;
}

SquareType Square::GetSquareType() const
{
    return id;
}

int Square::Distance(const Square& rhs) const
{
    int rankDist = ::abs(GetRank() - rhs.GetRank());
    int fileDist = ::abs(GetFile() - rhs.GetFile());
    return rankDist > fileDist ? rankDist : fileDist;
}

std::string Square::GetName() const
{
    if (id != NS)
        return std::string(1, GetFile() + 'a') + std::string(1, GetRank() + '1');
    else
        return "00";
}

std::ostream& operator<<(std::ostream& buf, const Square& sq)
{
    std::string Board[8];
    std::copy(EmptyBoard, EmptyBoard + 8, Board);
//    buf << std::string("Hello World!") << '\n';
    Board[sq.GetRank()][sq.GetFile()] = '#';
    for (int i = 7; i >= 0; --i)
        buf << Board[i] << '\n';
    return buf;
}
