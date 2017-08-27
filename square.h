#ifndef SQUARE_H
#define SQUARE_H

#include "base.h"

class Square
{
public:
    Square();
    
    Square(int file, int rank);
    
    Square(SquareType s);
    
    // Constructs Square from string (e.g. 'g6')
    Square(const std::string& s);
    
    Square(const Square& s);
    
    int File() const;
    
    int Rank() const;
    
    SquareType Id() const;

    //Distance to another square in king moves
    int Distance(const Square& s) const;
    
    std::string Name() const;
    
    friend std::ostream& operator<<(std::ostream& buf, const Square& sq);

private:
    SquareType id;    
};

#endif //SQUARE_H
