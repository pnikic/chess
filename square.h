#ifndef SQUARE_H
#define SQUARE_H

#include "base.h"

class Square
{
public:
    Square();
    Square(const int& file, const int& rank);
    Square(const SquareType& s);
    Square(const std::string& s);
    Square(const Square& s);
    ~Square();
    FileType File() const;
    RankType Rank() const;
    SquareType Id() const;
    int Distance(const Square& rhs) const;    //Distance to another square in king moves
    std::string Name() const;
    friend std::ostream& operator<<(std::ostream& buf, const Square& sq);

private:
    SquareType id;    
};

#endif //SQUARE_H
