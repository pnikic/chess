#include "piece.h"
#include <cctype>

Piece::Piece()
{
    id = NONE;
    color = WHITE;
}

Piece::Piece(PieceType type, Color c)
{
    ASSERT(type <= NONE && c <= BLACK, "Invalid piece type or color!");
    id = type;
    color = c;
}

Piece::Piece(char c)
{
    color = isupper(c) ? WHITE : BLACK;
    size_t f = PieceNames.find(toupper(c));
    ASSERT(f != std::string::npos, "Invalid piece type!");
    id = PieceTypes[f];
}

Piece::Piece(const Piece& p)
{
    id = p.id;
    color = p.color;
}

char Piece::Symbol() const
{
    for (int i = 0; i < 7; ++i)
        if (id == PieceTypes[i])
            return color ? tolower(PieceNames[i]) : PieceNames[i];
    CRASH("Invalid piece type!");
}

bool Piece::IsNone() const
{
    return id == NONE;
}

PieceType Piece::Type() const
{
    return id;
}

Color Piece::Side() const
{
    return color;
}
