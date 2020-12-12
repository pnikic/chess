#ifndef PIECE_H
#define PIECE_H

#include "base.h"

class Piece
{
public:
    Piece();

    Piece(PieceType type, Color c);

    // Constructs Piece from a char (e.g. 'k' or 'P').
    Piece(char c);

    Piece(const Piece &p);

    char Symbol() const;

    //char UnicodeSymbol() const;

    bool IsNone() const;

    PieceType Type() const;

    Color Side() const;

    bool operator==(const Piece &p) const;

    bool operator!=(const Piece &p) const;

    Piece& operator=(const Piece &p);

    friend std::ostream& operator<<(std::ostream &buf, const Piece &p);

private:
    PieceType id;
    Color color;
};

#endif //PIECE_H
