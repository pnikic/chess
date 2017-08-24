#include "square.h"

Square::Square()
{
    id = NS;
}

Square::Square(const int& file, const int& rank)
{
    assert(file >= 0 && rank >= 0 && file < 8 && rank < 8);
    id = rank * 8 + file;
}

Square::Square(const SquareType& s)
{
    assert(s <= H8);
    id = s;
}

Square::Square(const Square& s)
{
    id = s.id;
}

Square::Square(const std::string& s)
{
    assert(s.size() == 2);
    size_t file = FileNames.find(s[0]);
    size_t rank = RankNames.find(s[1]);
    assert (file != std::string::npos && rank != std::string::npos);
    id = rank * 8 + file;
}

Square::~Square(){}

FileType Square::File() const
{
    return id % 8;
}

RankType Square::Rank() const
{
    return id / 8;
}

SquareType Square::Id() const
{
    return id;
}

int Square::Distance(const Square& rhs) const
{
    int rankDist = ::abs(Rank() - rhs.Rank());
    int fileDist = ::abs(File() - rhs.File());
    return rankDist > fileDist ? rankDist : fileDist;
}

std::string Square::Name() const
{
    if (id != NS)
        return std::string(1, File() + 'a') + std::string(1, Rank() + '1');
    else
        return "00";
}

std::ostream& operator<<(std::ostream& buf, const Square& sq)
{
    std::string Board[8];
    std::copy(EmptyBoard, EmptyBoard + 8, Board);
    Board[sq.Rank()][sq.File()] = '1';
    for (int i = 7; i >= 0; --i)
        buf << Board[i] << '\n';
    return buf;
}
