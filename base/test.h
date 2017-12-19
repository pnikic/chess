#include <iostream>
#include <sstream>
#include <string>
#include "base.h"
#include "square.h"
#include "setsqr.h"
#include "piece.h"
#include "move.h"
#include "board.h"

class Test
{
public:
    Test();
    
    void test1();
    void test2();
    void test3();
    void test4();
    
private:
    Square e4, d5, a1;
    Piece A, B, C;
    Move m, n, o;
    SetSquares sA, sB;
    std::string test2Fen[8];
    std::string test3Fen[8];
    Board brd;
};
