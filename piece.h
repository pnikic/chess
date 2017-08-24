#ifndef PIECE_H
#define PIECE_H

#include "base.h"

class Piece
{
public:
    Piece();
    Piece(const PieceType& type, const Color& c);
    Piece(const char& s);
    Piece(const Piece& p);
    ~Piece();
    char Symbol();
    //UnicodeSymbol();

private:
    PieceType id;
    Color color;
};

#endif //PIECE_H
