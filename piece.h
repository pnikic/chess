#ifndef PIECE_H
#define PIECE_H

#include "base.h"

class Piece
{
public:
    Piece();
    Piece(PieceType type, Color c);
    Piece(char s);
    Piece(Piece& p);
    ~Piece();
    char GetSymbol();
    //GetUnicodeSymbol();

private:
    PieceType id;
    Color color;
};

#endif //PIECE_H
