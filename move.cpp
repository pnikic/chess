#include "move.h"
#include <cctype>

Move::Move()
{
    from = Square(NS);
    to = Square(NS);
    promotion = NONE;
}

Move::Move(Square& a, Square& b, PieceType p /*= NONE*/)
{
    assert (a.GetSquareType() <= NS && b.GetSquareType() <= NS && p <= NONE);
    from = a;
    to = b;
    promotion = p;
}

Move::Move(std::string s)
{
    assert(s.size() <= 5);
    unsigned file1 = FileNames.find(tolower(s[0])), rank1 = RankNames.find(tolower(s[1])), file2 = FileNames.find(tolower(s[2])), rank2 = RankNames.find(tolower(s[3]));
    assert (file1 != std::string::npos && file2 != std::string::npos && rank1 != std::string::npos && rank2 != std::string::npos);
    from = Square(file1, rank1);
    to = Square(file2, rank2);

    if (s.size() == 5)
    {
        unsigned ind = PieceNames.find(toupper(s[4]), 1);
        assert(ind != std::string::npos && ind != 5);
        promotion = PieceTypes[ind];
    }
    else
        promotion = NONE;
}

Move::Move(const Move& m)
{
    from = m.from;
    to = m.to;
    promotion = m.promotion;
}

Move::~Move(){};

std::string Move::GetUCI() const
{
    return from.GetName() + to.GetName() + (promotion != NONE ? std::string(1, tolower(PieceNames[promotion])) : "");
}
