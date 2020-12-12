#include "move.h"
#include <cctype>

Move::Move()
{
    from = Square(NS);
    to = Square(NS);
    promotion = NONE;
}

Move::Move(const Square &a, const Square &b, PieceType p /*= NONE*/)
{
    ASSERT (a.Id() <= NS && b.Id() <= NS && p <= NONE,
            "Invalid square(s) or promoted piece!");
    from = a;
    to = b;
    promotion = p;
}

Move::Move(const std::string &s)
{
    ASSERT(s.size() <= 5, "Invalid UCI move: too long!");
    size_t file1 = FileNames.find(tolower(s[0])), rank1 = RankNames.find(tolower(s[1]));
    size_t file2 = FileNames.find(tolower(s[2])), rank2 = RankNames.find(tolower(s[3]));
    ASSERT(file1 != std::string::npos && file2 != std::string::npos &&
           rank1 != std::string::npos && rank2 != std::string::npos,
           "Invlid UCI move: invalid number(s) or letter(s)!");
    from = Square(rank1, file1);
    to = Square(rank2, file2);

    if (s.size() == 5)
    {
        size_t ind = PieceNames.find(toupper(s[4]), 1);
        ASSERT(ind != std::string::npos && ind != 5,
               "Invalic UCI move: invalid promoted piece!");
        promotion = PieceTypes[ind];
    }
    else
        promotion = NONE;
}

Move::Move(const Move &m)
{
    from = m.from;
    to = m.to;
    promotion = m.promotion;
}

std::string Move::UCI() const
{
    return from.Name() + to.Name() +
        (promotion != NONE ? std::string(1, tolower(PieceNames[promotion])) : "");
}

Square Move::From() const
{
    return from;
}

Square Move::To() const
{
    return to;
}

PieceType Move::Promotion() const
{
    return promotion;
}

Move& Move::operator=(const Move& m)
{
    // Self-assignment check
    if (this != &m)
    {
        from = m.from;
        to = m.to;
        promotion = m.promotion;
    }

    return *this;
}
