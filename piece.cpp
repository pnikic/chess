#include "piece.h"
#include <cctype>

Piece::Piece()
{
    id = NONE;
}

Piece::Piece(const PieceType& type, const Color& c)
{
    assert(type <= NONE);
    id = type;
    color = c;
}

Piece::Piece(const char& s)
{
    color = isupper(s);
    char S = toupper(s);
    for (int i = 0; i < 7; ++i)
        if (S == PieceNames[i])
        {
            id = PieceTypes[i];
            return;
        }
    assert(false);
}

Piece::Piece(const Piece& p)
{
    id = p.id;
    color = p.color;
}

Piece::~Piece(){}

char Piece::Symbol()
{
    for (int i = 0; i < 7; ++i)
        if (id == PieceTypes[i])
            return color ? PieceNames[i] : tolower(PieceNames[i]);
    assert(false);
}
