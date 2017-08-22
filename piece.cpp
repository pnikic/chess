#include "piece.h"
#include <cctype>

Piece::Piece()
{
    id = NONE;
}

Piece::Piece(PieceType type, Color c)
{
    assert(type <= NONE);
    id = type;
    color = c;
}

Piece::Piece(char s)
{
    color = isupper(s);
    s = toupper(s);
    for (int i = 0; i < 7; ++i)
        if (s == PieceNames[i])
        {
            id = PieceTypes[i];
            return;
        }
    assert(false);
}

Piece::Piece(Piece& p)
{
    id = p.id;
    color = p.color;
}

Piece::~Piece(){}

char Piece::GetSymbol()
{
    for (int i = 0; i < 7; ++i)
        if (id == PieceTypes[i])
            return color ? PieceNames[i] : tolower(PieceNames[i]);
}
