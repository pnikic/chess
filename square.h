#ifndef SQUARE_H
#define SQUARE_H

#include "base.h"

class Square
{
public:
    Square();
    Square(int file, int rank);
    Square(SquareType s);
    Square(Square& s);
    ~Square();
    FileType GetFile() const;
    RankType GetRank() const;
    SquareType GetSquareType() const;
    int Distance(const Square& rhs) const;    //Distance to another square in king moves
    std::string GetName() const;
    friend std::ostream& operator<<(std::ostream& buf, const Square& sq);

private:
    SquareType id;    
};

#endif //SQUARE_H
