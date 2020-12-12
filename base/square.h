#ifndef SQUARE_H
#define SQUARE_H

#include "base.h"

class Square
{
public:
    Square();

    // Construct a square from a rank and file.
    // rank = 0, ..., 7 = from 1st to 8th rank
    // file = 0, ..., 7  =from a- to h-file
    Square(int rank, int file);

    // Constructs a square from a bitboard with one active bit
    Square(BitBoard b);

    Square(SquareType s);

    // Constructs a Square from string (e.g. 'g6').
    Square(const std::string &s);

    Square(const Square &s);

    int File() const;

    int Rank() const;

    SquareType Id() const;

    BitBoard BB() const;

    // Returns the distance to another square in king moves.
    int Distance(const Square &s) const;

    // Checks if the square lies on the same rank / file / diagonal with another square.
    // If it does, returns the direction index, otherwise -1.
    int Direction(const Square &s) const;

    std::string Name() const;

    bool operator==(const Square &sq) const;

    bool operator!=(const Square &sq) const;

    Square& operator=(const Square &sq);

    friend std::ostream& operator<<(std::ostream &buf, const Square &sq);

private:
    SquareType id;
};

#endif //SQUARE_H
